#pragma once
#include "Gaag.h"

PREDEFINE(Water, pWater);
PREDEFINE(HeightField, pHeightField);
PREDEFINE(WaterTile, pWaterTile);
PREDEFINE(Material, pMaterial);

class Water
{
public:
	Water() { mTerrainHeightField = nullptr; mWaterHeightField = nullptr; mWaterTiles = nullptr; }
	~Water() { CleanUp(); }

	void Init(pHeightField inHeightField, pMaterial inMaterial);
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

