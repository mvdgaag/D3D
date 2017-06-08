#pragma once
#include "Gaag.h"

REGISTERCLASS(Water);
REGISTERCLASS(Terrain);
REGISTERCLASS(WaterTile);
REGISTERCLASS(Material);

class Water
{
public:
	Water() { mTerrain = nullptr; mWaterSurface = nullptr; mWaterTiles = nullptr; }
	~Water() { CleanUp(); }

	void Init(pTerrain inTerrain, pMaterial inMaterial, pMaterial inShadowMaterial);
	void CleanUp();
	void Update(float inTimeStep);

	pTerrain GetTerrain() { return mTerrain; }
	pTerrain GetWaterSurface() { return mWaterSurface; }
	pWaterTile GetTile(int2 inTileCoord) { return mWaterTiles[inTileCoord.x][inTileCoord.y]; }

private:
	int2 mNumTiles;
	pTerrain mTerrain;
	pTerrain mWaterSurface;
	pWaterTile** mWaterTiles;
};

