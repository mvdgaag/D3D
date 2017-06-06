#pragma once
#include "Gaag.h"
#include "TerrainTile.h"
#include "Layer.h"


REGISTERCLASS(Terrain);

class Terrain
{
public:
	Terrain() {};
	~Terrain() {};
	void Init(int2 inNumTiles, int2 inTileSegments, float3 inTileScale, pMaterial inMaterial, pMaterial inShadowMaterial);
	void CleanUp();

	int2			GetNumTiles()				{ return mNumTiles; }
	int2			GetTileSegments()			{ return mTileSegments; }
	float3			GetTileScale()				{ return mTileScale; }
	
	
	float2 WorldToTileSpace(float2 inWorldCoord)
	{
		float2 world_top_left = float2(mNumTiles.x, mNumTiles.y) * float2(mTileScale.x, mTileScale.y) / 2.0f;
		float2 offset_coord = inWorldCoord + world_top_left;
		return offset_coord / float2(mTileScale.x, mTileScale.y);
	}


	pTerrainTile GetTile(const int2& inTileIndex)	
	{ 
		if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y)
			return mTiles[inTileIndex.y * mNumTiles.x + inTileIndex.x];
		else
			return nullptr;
	}


	pRenderTarget GetLayerRenderTarget(const int2& inTileIndex, const int inLayerID)
	{
		if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y && inLayerID < mLayers.size())
			return mLayers[inLayerID]->GetTileRenderTarget(inTileIndex);
		else
			return nullptr;
	}


	pTexture GetLayerTexture(const int2& inTileIndex, const int inLayerID)
	{
		if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y && inLayerID < mLayers.size())
			return mLayers[inLayerID]->GetTileTexture(inTileIndex);
		else
			return nullptr;
	}

	
	pTerrainTile GetTile(const float2& inWorldCoord)
	{
		int2 tile_index = WorldToTileSpace(inWorldCoord);
		return GetTile(tile_index);
	}


	pRenderTarget GetLayerRenderTarget(const float2& inWorldCoord, const int inLayerID)
	{
		int2 tile_index = WorldToTileSpace(inWorldCoord);
		return GetLayerRenderTarget(tile_index, inLayerID);
	}


	pTexture GetLayerTexture(const float2& inWorldCoord, const int inLayerID)
	{
		int2 tile_index = WorldToTileSpace(inWorldCoord);
		return GetLayerTexture(tile_index, inLayerID);
	}


	std::vector<int2> GetTiles(const rect& inWorldRect)
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


private:
	int2 mNumTiles;
	int2 mTileSegments;
	float3 mTileScale;
	pTerrainTile* mTiles = nullptr;
	apLayer mLayers;
};

