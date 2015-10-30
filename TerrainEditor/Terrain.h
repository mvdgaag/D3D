#pragma once
#include "GaagCommon.h"

PREDEFINE(Terrain, pTerrain);
PREDEFINE(TerrainTile, pTerrainTile);
PREDEFINE(Material, pMaterial);


class Terrain
{
public:
	Terrain() {};
	~Terrain() {};
	void Init(int2 inNumTiles, int2 inTileSegments, float3 inTileScale, pMaterial inMaterial);
	void CleanUp();
private:
	int2 mNumTiles;
	int2 mTileSegments;
	float3 mTileScale;
	pTerrainTile* mTiles = nullptr;
};

