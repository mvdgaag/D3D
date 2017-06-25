#include "Terrain.h"
#include "TerrainTile.h"


// declare static variables
pComputeShader Terrain::sUpdateNormalShader;




void Terrain::Init(int2 inNumTiles, int2 inTileResolution, float3 inTileScale, pMaterial inMaterial, pMaterial inShadowMaterial)
{
	CleanUp();
	
	if (sUpdateNormalShader == nullptr)
		sUpdateNormalShader = theResourceFactory.LoadComputeShader("Shaders/TerrainUpdateNormalShader.hlsl");

	mNumTiles = inNumTiles;
	mTileResolution = inTileResolution;
	mTileScale = inTileScale;

	mLayers.resize(2);

	pLayer height_layer = new Layer();
	height_layer->Init(inNumTiles, int2(mTileResolution.x, mTileResolution.y), FORMAT_R32_FLOAT, LayerType::LAYER_HEIGHT);
	mLayers[0] = height_layer;
	mHeightLayerIndex = 0;

	pLayer normal_layer = new Layer();
	normal_layer->Init(inNumTiles, int2(mTileResolution.x, mTileResolution.y), FORMAT_R16G16B16A16_FLOAT, LayerType::LAYER_NORMAL);
	mLayers[1] = normal_layer;
	mNormalLayerIndex = 1;

	mTiles = new pTerrainTile[mNumTiles.x * mNumTiles.y];
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int idx = y * mNumTiles.x + x;
			
			float3 tile_pos;
			tile_pos.x = (float(x) - float(mNumTiles.x - 1) / 2.0) * mTileScale.x;
			tile_pos.y = 0.0;
			tile_pos.z = (float(y) - float(mNumTiles.y - 1) / 2.0) * mTileScale.y;
			
			mTiles[idx] = MAKE_NEW(TerrainTile);
			pMaterial material = theResourceFactory.CloneMaterial(inMaterial);
			
			material->SetDiffuseValue(float4(0.7, 0.7, 0.7, 0));

			mTiles[idx]->Init(this, int2(x, y), material, inShadowMaterial);
			
			Gaag.RegisterObject(mTiles[idx]);
		}
	}

	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(mConstantBufferData));
	mConstantBufferData.scale = float4(mTileScale, 0.0);
	mConstantBufferData.textureInfo = int4(mTileResolution, 0, 0);
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);
}


void Terrain::CleanUp()
{
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int idx = y * mNumTiles.x + x;
			mTiles[idx] = nullptr;
		}
	}
	delete[] mTiles;
	mTiles = nullptr;

	for each (pLayer layer in mLayers)
		layer->CleanUp();
	mLayers.clear();

	mConstantBuffer = nullptr;
}


pLayer Terrain::GetLayer(int inIndex)
{
	assert(ValidateLayerIndex(inIndex));
	return mLayers[inIndex];
}


int Terrain::SetLayer(pLayer inLayer, int inIndex)
{
	if (inIndex == -1)
		inIndex = mLayers.size();

	if (inIndex >= mLayers.size())
		mLayers.resize(inIndex + 1);

	assert(mLayers[inIndex] == nullptr);
	mLayers[inIndex] = inLayer;
	return inIndex;
}


int Terrain::FindLayer(pLayer inLayer)
{
	for (int i = 0; i < mLayers.size(); i++)
	{
		if (mLayers[i] == inLayer);
			return i;
	}
	return -1;
}


void Terrain::UpdateNormals(pRenderTarget inTarget, apTexture inHeights)
{
	assert(inHeights.size() == 5);

	theRenderContext.CSSetShader(sUpdateNormalShader);
	theRenderContext.CSSetRWTexture(inTarget, 0, 0);
	theRenderContext.CSSetTexture(inHeights[0], 0); // c
	theRenderContext.CSSetTexture(inHeights[1], 1); // n
	theRenderContext.CSSetTexture(inHeights[2], 2); // e
	theRenderContext.CSSetTexture(inHeights[3], 3); // s
	theRenderContext.CSSetTexture(inHeights[4], 4); // w

	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);
	int2 num_threads = (inTarget->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 1);
	theRenderContext.CSSetTexture(NULL, 2);
	theRenderContext.CSSetTexture(NULL, 3);
	theRenderContext.CSSetTexture(NULL, 4);
	theRenderContext.CSSetShader(NULL);
}


void Terrain::SetDirty(int2 inTileIndex, int inLayerIndex)
{
	assert(ValidateLayerIndex(inLayerIndex));
	assert(mLayers[inLayerIndex] != nullptr);

	mLayers[inLayerIndex]->SetDirty(inTileIndex);

	// If height is dirty so should the normals be.
	if (inLayerIndex == mHeightLayerIndex)
		SetDirty(inTileIndex, mNormalLayerIndex);

}


void Terrain::ProcessDirtyLayers()
{
	for each (pLayer layer in mLayers)
	{
		int2 num_tiles = layer->GetNumTiles();
		for (int y = 0; y < num_tiles.y; y++)
		{
			for (int x = 0; x < num_tiles.y; x++)
			{
				if (layer->GetDirty(int2(x, y)))
				{
					if (layer->GetType() == LayerType::LAYER_NORMAL)
					{
						apTexture heights;
						heights.push_back(GetLayerTexture(int2(x, y), mHeightLayerIndex));
						heights.push_back(GetLayerTexture(int2(x, y + 1), mHeightLayerIndex));
						heights.push_back(GetLayerTexture(int2(x + 1, y), mHeightLayerIndex));
						heights.push_back(GetLayerTexture(int2(x, y - 1), mHeightLayerIndex));
						heights.push_back(GetLayerTexture(int2(x - 1, y), mHeightLayerIndex));
						UpdateNormals(GetLayerRenderTarget(int2(x, y), mNormalLayerIndex), heights);
					}
					layer->SetDirty(int2(x, y), false);
				}
			}
		}
	}
}


void Terrain::SetLayerAsAlbedo(int inLayerIndex)
{
	assert(ValidateLayerIndex(inLayerIndex));
	assert(mLayers[inLayerIndex] != nullptr);

	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.y; x++)
		{
			int idx = y * mNumTiles.x + x;
			mTiles[idx]->GetMaterial()->SetDiffuseTexture(mLayers[inLayerIndex]->GetTileTexture(int2(x, y)));
		}
	}
}


void Terrain::SetLayerAsHeight(int inLayerIndex)
{
	assert(ValidateLayerIndex(inLayerIndex));
	assert(mLayers[inLayerIndex] != nullptr);

	mHeightLayerIndex = inLayerIndex;
}


float2 Terrain::WorldToTileSpace(float2 inWorldCoord)
{
	float2 world_top_left = float2(mNumTiles.x, mNumTiles.y) * float2(mTileScale.x, mTileScale.y) / 2.0f;
	return (inWorldCoord + world_top_left) / float2(mTileScale.x, mTileScale.y);
}


float2 Terrain::TileToWorldSpace(float2 inTileCoord)
{
	float2 world_top_left = float2(mNumTiles.x, mNumTiles.y) * float2(mTileScale.x, mTileScale.y) / 2.0f;
	return inTileCoord * float2(mTileScale.x, mTileScale.y) - world_top_left;
}


pTerrainTile Terrain::GetTile(const int2& inTileIndex)
{
	if (ValidateTileIndex(inTileIndex))
		return mTiles[inTileIndex.y * mNumTiles.x + inTileIndex.x];
	else
		return nullptr;
}


pTerrainTile Terrain::GetTile(const float2& inWorldCoord)
{
	int2 tile_index = WorldToTileSpace(inWorldCoord);
	return GetTile(tile_index);
}


pRenderTarget Terrain::GetLayerRenderTarget(const int2& inTileIndex, const int inLayerIndex)
{
	if (ValidateTileIndex(inTileIndex) && ValidateLayerIndex(inLayerIndex))
		return mLayers[inLayerIndex]->GetTileRenderTarget(inTileIndex);
	else
		return nullptr;
}


pRenderTarget Terrain::GetLayerRenderTarget(const float2& inWorldCoord, const int inLayerIndex)
{
	int2 tile_index = WorldToTileSpace(inWorldCoord);
	return GetLayerRenderTarget(tile_index, inLayerIndex);
}


pTexture Terrain::GetLayerTexture(const int2& inTileIndex, const int inLayerIndex)
{
	if (ValidateTileIndex(inTileIndex) && ValidateLayerIndex(inLayerIndex))
		return mLayers[inLayerIndex]->GetTileTexture(inTileIndex);
	else
		return nullptr;
}


pTexture Terrain::GetLayerTexture(const float2& inWorldCoord, const int inLayerIndex)
{
	int2 tile_index = WorldToTileSpace(inWorldCoord);
	return GetLayerTexture(tile_index, inLayerIndex);
}


std::vector<int2> Terrain::GetTiles(const rect& inWorldRect)
{
	float2 tile_min = float2(0.0, 0.0);
	float2 tile_max = float2(mNumTiles);
	float2 top_left = glm::clamp(WorldToTileSpace(inWorldRect.topLeft), tile_min, tile_max);
	float2 bottom_right = glm::clamp(WorldToTileSpace(inWorldRect.bottomRight), tile_min, tile_max);

	std::vector<int2> result;
	for (int y = top_left.y; y < bottom_right.y; y++)
		for (int x = top_left.x; x < bottom_right.x; x++)
			result.push_back(int2(x, y));

	return result;
}