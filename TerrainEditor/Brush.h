#pragma once
#include "Gaag.h"
#include "TerrainTile.h"

PREDEFINE(Brush, pBrush);

class Brush
{
public:
	Brush() {};
	~Brush() {};
	void Apply(pTerrainTile inTileNW, pTerrainTile inTileNE, pTerrainTile inTileSW, pTerrainTile inTileSE, int2 inPixelCoord);
private:
	float mRadius;
	pComputeShader mShader;
};

