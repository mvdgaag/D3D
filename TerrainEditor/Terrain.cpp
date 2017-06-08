#include "Terrain.h"
#include "TerrainTile.h"


void Terrain::Init(int2 inNumTiles, int2 inTileSegments, float3 inTileScale, pMaterial inMaterial, pMaterial inShadowMaterial)
{
	CleanUp();
	
	mNumTiles = inNumTiles;
	mTileSegments = inTileSegments;
	mTileScale = inTileScale;

	pLayer height_layer = new Layer();
	height_layer->Init(inNumTiles, int2(mTileSegments.x + 1, mTileSegments.y + 1), FORMAT_R32_FLOAT);
	mLayers.push_back(height_layer);

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
			material->SetDiffuseValue(float4(1, 0, 0, 0));
			mTiles[idx]->Init(tile_pos, mTileScale, mTileSegments, material, inShadowMaterial, height_layer->GetTileTexture(int2(x, y)));
			
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


void Terrain::SetLayer(pLayer inLayer, int inIndex)
{
	assert(inIndex != 0);
	if (inIndex >= mLayers.size())
		mLayers.resize(inIndex + 1);
	assert(mLayers[inIndex] == nullptr);
	mLayers[inIndex] = inLayer;
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


pRenderTarget Terrain::GetLayerRenderTarget(const int2& inTileIndex, const int inLayerID)
{
	if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y && inLayerID < mLayers.size())
		return mLayers[inLayerID]->GetTileRenderTarget(inTileIndex);
	else
		return nullptr;
}


pTexture Terrain::GetLayerTexture(const int2& inTileIndex, const int inLayerID)
{
	if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y && inLayerID < mLayers.size())
		return mLayers[inLayerID]->GetTileTexture(inTileIndex);
	else
		return nullptr;
}


pTerrainTile Terrain::GetTile(const float2& inWorldCoord)
{
	int2 tile_index = WorldToTileSpace(inWorldCoord);
	return GetTile(tile_index);
}


pRenderTarget Terrain::GetLayerRenderTarget(const float2& inWorldCoord, const int inLayerID)
{
	int2 tile_index = WorldToTileSpace(inWorldCoord);
	return GetLayerRenderTarget(tile_index, inLayerID);
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