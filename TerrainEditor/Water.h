#pragma once
#include "Gaag.h"

PREDEFINE(Water, pWater);
PREDEFINE(Terrain, pTerrain);
PREDEFINE(WaterTile, pWaterTile);
PREDEFINE(Material, pMaterial);

class Water
{
public:
	Water() { mHeightField = nullptr; mWaterTiles = nullptr; }
	~Water() { CleanUp(); }

	void Init(pTerrain inTerrain, pMaterial inMaterial);
	void CleanUp();
	void Update();

private:
	int2 mNumTiles;
	pTerrain mTerrain;
	pTerrain mHeightField;
	pWaterTile** mWaterTiles;
};

