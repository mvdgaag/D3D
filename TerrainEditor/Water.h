#pragma once
#include "Gaag.h"

REGISTERCLASS(Water);
REGISTERCLASS(HeightField);
REGISTERCLASS(WaterTile);
REGISTERCLASS(Material);

class Water
{
public:
	Water() { mTerrainHeightField = nullptr; mWaterHeightField = nullptr; mWaterTiles = nullptr; }
	~Water() { CleanUp(); }

	void Init(pHeightField inHeightField, pMaterial inMaterial, pMaterial inShadowMaterial);
	void CleanUp();
	void Update(float inTimeStep);

	pHeightField GetTerrainHeightField() { return mTerrainHeightField; }
	pHeightField GetWaterHeightField() { return mWaterHeightField; }
	pWaterTile GetTile(int2 inTileCoord) { return mWaterTiles[inTileCoord.x][inTileCoord.y]; }


private:
	int2 mNumTiles;
	pHeightField mTerrainHeightField;
	pHeightField mWaterHeightField;
	pWaterTile** mWaterTiles;
};

