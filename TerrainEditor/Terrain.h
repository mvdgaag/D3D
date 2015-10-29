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
	void Init(int inWidth, int inHeight, int inTileWidth, int inTileHeight, pMaterial inMaterial);
	void CleanUp();
private:
	int mWidth = 0;
	int mHeight = 0;
	pTerrainTile* mTiles = nullptr;
};

