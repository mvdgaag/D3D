#include "Water.h"
#include "Terrain.h"
#include "WaterTile.h"


void Water::Init(pTerrain inTerrainTerrain, pMaterial inMaterial, pMaterial inShadowMaterial)
{
	assert(inTerrainTerrain != nullptr);
	mTerrainTerrain = inTerrainTerrain;

	mWaterTerrain = MAKE_NEW(Terrain);
	mWaterTerrain->Init(mTerrainTerrain->GetNumTiles(), mTerrainTerrain->GetTileSegments(), mTerrainTerrain->GetTileScale(), inMaterial, inShadowMaterial);

	mNumTiles = mWaterTerrain->GetNumTiles();
	mWaterTiles = new pWaterTile*[mNumTiles.x];
	for (int x = 0; x < mNumTiles.x; x++)
	{
		mWaterTiles[x] = new pWaterTile[mNumTiles.y];
		for (int y = 0; y < mNumTiles.y; y++)
		{
			int2 tile_coord(x, y);
			pTexture terrain_height_texture = mTerrainTerrain->GetTile(tile_coord)->GetHeightTexture();
			pTexture water_height_texture = mWaterTerrain->GetTile(tile_coord)->GetHeightTexture();
			
			pWaterTile water_tile = MAKE_NEW(WaterTile);
			water_tile->Init(terrain_height_texture, water_height_texture, 
				mWaterTerrain->GetTileScale().x / water_height_texture->GetDimensions().x, // pixelscale assumes square pixels
				mWaterTerrain->GetTileScale().z);

			mWaterTiles[x][y] = water_tile;
		}
	}
}


void Water::CleanUp()
{
	for (int x = 0; x < mNumTiles.x; x++)
	{
		for (int y = 0; y < mNumTiles.y; y++)
		{
			mWaterTiles[x][y] = nullptr;
		}
		delete[] mWaterTiles[x];
	}
	delete[] mWaterTiles;

	mWaterTerrain = nullptr;
	mTerrainTerrain = nullptr;
}


void Water::Update(float inTimeStep)
{
	// update water height for all tiles
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			pWaterTile north = nullptr;
			pWaterTile east = nullptr;
			pWaterTile south = nullptr;
			pWaterTile west = nullptr;

			south = mWaterTiles[x][(y + mNumTiles.y - 1) % mNumTiles.y];
			west = mWaterTiles[(x + mNumTiles.x - 1) % mNumTiles.x][y];
			east = mWaterTiles[(x + 1) % mNumTiles.x][y];
			north = mWaterTiles[x][(y + 1) % mNumTiles.y];

			mWaterTiles[x][y]->UpdateWater(north, east, south, west);
		}
	}

	// update flux for all tiles
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			pWaterTile north = nullptr;
			pWaterTile east = nullptr;
			pWaterTile south = nullptr;
			pWaterTile west = nullptr;

			south = mWaterTiles[x][(y + mNumTiles.y - 1) % mNumTiles.y];
			west = mWaterTiles[(x + mNumTiles.x - 1) % mNumTiles.x][y];
			east = mWaterTiles[(x + 1) % mNumTiles.x][y];
			north = mWaterTiles[x][(y + 1) % mNumTiles.y];

			mWaterTiles[x][y]->UpdateFlux(north, east, south, west);
		}
	}
}