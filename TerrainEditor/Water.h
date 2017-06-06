#pragma once
#include "Gaag.h"

REGISTERCLASS(Water);
REGISTERCLASS(Terrain);
REGISTERCLASS(WaterTile);
REGISTERCLASS(Material);

class Water
{
public:
	Water() { mTerrainTerrain = nullptr; mWaterTerrain = nullptr; mWaterTiles = nullptr; }
	~Water() { CleanUp(); }

	void Init(pTerrain inTerrain, pMaterial inMaterial, pMaterial inShadowMaterial);
	void CleanUp();
	void Update(float inTimeStep);

	pTerrain GetTerrainTerrain() { return mTerrainTerrain; }
	pTerrain GetWaterTerrain() { return mWaterTerrain; }
	pWaterTile GetTile(int2 inTileCoord) { return mWaterTiles[inTileCoord.x][inTileCoord.y]; }


private:
	int2 mNumTiles;
	pTerrain mTerrainTerrain;
	pTerrain mWaterTerrain;
	pWaterTile** mWaterTiles;
};

