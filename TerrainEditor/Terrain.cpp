#include "Terrain.h"
#include "TerrainTile.h"


void Terrain::Init(int2 inNumTiles, int2 inTileSegments, float3 inTileScale, pMaterial inMaterial, pMaterial inShadowMaterial)
{
	CleanUp();
	
	mNumTiles = inNumTiles;
	mTileSegments = inTileSegments;
	mTileScale = inTileScale;

	assert(LayerType::LAYER_HEIGHT == 0 && LayerType::LAYER_NORMAL == 1);
	mLayers.resize(2);

	pLayer height_layer = new Layer();
	height_layer->Init(inNumTiles, int2(mTileSegments.x, mTileSegments.y), FORMAT_R32_FLOAT, LayerType::LAYER_HEIGHT);
	mLayers[(int)LayerType::LAYER_HEIGHT] = height_layer;
	
	pLayer normal_layer = new Layer();
	normal_layer->Init(inNumTiles, int2(mTileSegments.x, mTileSegments.y), FORMAT_R16G16B16A16_FLOAT, LayerType::LAYER_NORMAL);
	mLayers[(int)LayerType::LAYER_NORMAL] = normal_layer;

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

			// DEVHACK
			material->SetDiffuseTexture(height_layer->GetTileTexture(int2(x, y)));

			mTiles[idx]->Init(tile_pos, mTileScale, mTileSegments, material, inShadowMaterial, height_layer->GetTileTexture(int2(x, y)), normal_layer->GetTileTexture(int2(x, y)));
			
			Gaag.RegisterObject(mTiles[idx]);
		}
	}
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
}


pLayer Terrain::GetLayer(int inIndex)
{
	return mLayers[inIndex];
}


void Terrain::SetLayer(pLayer inLayer, int inIndex = -1)
{
	assert(inIndex != 0);

	if (inIndex == -1)
		inIndex = mLayers.size();

	if (inIndex >= mLayers.size())
		mLayers.resize(inIndex + 1);

	assert(mLayers[inIndex] == nullptr);
	mLayers[inIndex] = inLayer;
}


void Terrain::SetDirty(int2 inTileIndex, int inLayerIndex)
{
	assert(inLayerIndex < mLayers.size());
	assert(mLayers[inLayerIndex] != nullptr);

	mLayers[inLayerIndex]->SetDirty(inTileIndex);

	// If height is dirty so should the normals be.
	if (inLayerIndex == (int)LayerType::LAYER_HEIGHT)
		SetDirty(inTileIndex, (int)LayerType::LAYER_NORMAL);

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
						apTexture neighbors;
						neighbors.push_back(GetLayerTexture(int2(x, y + 1), (int)LayerType::LAYER_HEIGHT));
						neighbors.push_back(GetLayerTexture(int2(x + 1, y), (int)LayerType::LAYER_HEIGHT));
						neighbors.push_back(GetLayerTexture(int2(x, y - 1), (int)LayerType::LAYER_HEIGHT));
						neighbors.push_back(GetLayerTexture(int2(x - 1, y), (int)LayerType::LAYER_HEIGHT));
						mTiles[y * mNumTiles.x + x]->UpdateNormals(neighbors);
					}
					layer->SetDirty(int2(x, y), false);
				}
			}
		}
	}
}


float2 Terrain::WorldToTileSpace(float2 inWorldCoord)
{
	float2 world_top_left = float2(mNumTiles.x, mNumTiles.y) * float2(mTileScale.x, mTileScale.y) / 2.0f;
	float2 offset_coord = inWorldCoord + world_top_left;
	return offset_coord / float2(mTileScale.x, mTileScale.y);
}


pTerrainTile Terrain::GetTile(const int2& inTileIndex)
{
	if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y)
		return mTiles[inTileIndex.y * mNumTiles.x + inTileIndex.x];
	else
		return nullptr;
}


pTerrainTile Terrain::GetTile(const float2& inWorldCoord)
{
	int2 tile_index = WorldToTileSpace(inWorldCoord);
	return GetTile(tile_index);
}


pRenderTarget Terrain::GetLayerRenderTarget(const int2& inTileIndex, const int inLayerID)
{
	if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y && inLayerID < mLayers.size())
		return mLayers[inLayerID]->GetTileRenderTarget(inTileIndex);
	else
		return nullptr;
}


pRenderTarget Terrain::GetLayerRenderTarget(const float2& inWorldCoord, const int inLayerID)
{
	int2 tile_index = WorldToTileSpace(inWorldCoord);
	return GetLayerRenderTarget(tile_index, inLayerID);
}


pTexture Terrain::GetLayerTexture(const int2& inTileIndex, const int inLayerID)
{
	if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y && inLayerID < mLayers.size())
		return mLayers[inLayerID]->GetTileTexture(inTileIndex);
	else
		return nullptr;
}


pTexture Terrain::GetLayerTexture(const float2& inWorldCoord, const int inLayerID)
{
	int2 tile_index = WorldToTileSpace(inWorldCoord);
	return GetLayerTexture(tile_index, inLayerID);
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