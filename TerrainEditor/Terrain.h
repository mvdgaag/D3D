#pragma once
#include "Gaag.h"
#include "TerrainTile.h"

PREDEFINE(Terrain, pTerrain);

class Terrain
{
public:
	Terrain() {};
	~Terrain() {};
	void Init(int2 inNumTiles, int2 inTileSegments, float3 inTileScale, pMaterial inMaterial);
	void CleanUp();

	int2			GetNumTiles()				{ return mNumTiles; }
	int2			GetTileSegments()			{ return mTileSegments; }
	float3			GetTileScale()				{ return mTileScale; }

	bool ToTileSpace(float2 inWorldCoord, int2& outTileIndex, int2& outPixelCoord)
	{
		float2 world_top_left = float2(mNumTiles.x, mNumTiles.y) * float2(mTileScale.x, mTileScale.y) / 2.0f;
		float2 offset_coord = inWorldCoord + world_top_left;
		float2 tile_index = offset_coord / float2(mTileScale.x, mTileScale.y);
		
		if (tile_index.x < 0 || tile_index.x >= mNumTiles.x || tile_index.y < 0 || tile_index.y >= mNumTiles.y)
			return false;
		
		outTileIndex = tile_index;

		float2 tile_uv = glm::modf(tile_index, float2(1.0f, 1.0f));
		int2 resolution = GetTile(outTileIndex)->GetTexture()->GetResolution();
		float2 tile_pixel = tile_uv * float2(resolution);

		outPixelCoord = tile_pixel;
		outPixelCoord %= resolution;
		return true;
	}

	pTerrainTile GetTile(int2 inTileIndex)	
	{ 
		if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y)
			return mTiles[inTileIndex.y * mNumTiles.x + inTileIndex.x];
		else
			return nullptr;
	}

private:
	int2 mNumTiles;
	int2 mTileSegments;
	float3 mTileScale;
	pTerrainTile* mTiles = nullptr;
};

