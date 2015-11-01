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
	pTerrainTile	GetTile(int2 inTileIndex)	{ return mTiles[inTileIndex.y * mNumTiles.x + inTileIndex.x]; }
	pTerrainTile	GetTile(float2 inWorldCoord)	
	{ 
		int2 tile_index = inWorldCoord / mTileScale.XY();
		return mTiles[tile_index.y * mNumTiles.x + tile_index.x];
	}
	float2			GetWorldCenter()			{ return float2(float2(mNumTiles.x * mTileScale.x, mNumTiles.y * mTileScale.y) / 2.0); }

private:
	int2 mNumTiles;
	int2 mTileSegments;
	float3 mTileScale;
	pTerrainTile* mTiles = nullptr;
};

