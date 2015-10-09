#pragma once

class TerrainTile;


class Terrain
{
public:
	Terrain();
	~Terrain();
private:
	int mWidth, mHeight;
	TerrainTile** mTiles;
};

