#include "Terrain.h"
#include "TerrainTile.h"


void Terrain::Init(int2 inNumTiles, int2 inTileSegments, float3 inTileScale, pMaterial inMaterial)
{
	CleanUp();
	
	mNumTiles = inNumTiles;
	mTileSegments = inTileSegments;
	mTileScale = inTileScale;

	mTiles = new pTerrainTile[mNumTiles.x * mNumTiles.y];
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int idx = y * mNumTiles.x + x;
			
			float3 tile_pos;
			tile_pos.x = (float(x) - float(mNumTiles.x) / 2.0) * mTileScale.x;
			tile_pos.y = 0.0;
			tile_pos.z = (float(y) - float(mNumTiles.y) / 2.0) * mTileScale.y;

			mTiles[idx] = MAKE_NEW(TerrainTile);
			mTiles[idx]->Init(tile_pos, mTileScale, mTileSegments, inMaterial);
			
			Gaag.RegisterObject(mTiles[idx]);
		}
	}
}


void Terrain::CleanUp()
{
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int idx = y * mNumTiles.x + x;
			mTiles[idx] = nullptr;
		}
	}
	delete[] mTiles;
	mTiles = nullptr;
}
