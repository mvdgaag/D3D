#include "Water.h"
#include "HeightField.h"
#include "WaterTile.h"


void Water::Init(pHeightField inTerrainHeightField, pMaterial inMaterial)
{
	assert(inTerrainHeightField != nullptr);
	mTerrainHeightField = inTerrainHeightField;

	mWaterHeightField = MAKE_NEW(HeightField);
	mWaterHeightField->Init(mTerrainHeightField->GetNumTiles(), mTerrainHeightField->GetTileSegments(), mTerrainHeightField->GetTileScale(), inMaterial);

	mNumTiles = mWaterHeightField->GetNumTiles();
	mWaterTiles = new pWaterTile*[mNumTiles.x];
	for (int x = 0; x < mNumTiles.x; x++)
	{
		mWaterTiles[x] = new pWaterTile[mNumTiles.y];
		for (int y = 0; y < mNumTiles.y; y++)
		{
			int2 tile_coord(x, y);
			pTexture terrain_height_texture = mTerrainHeightField->GetTile(tile_coord)->GetHeightTexture();
			pTexture water_height_texture = mWaterHeightField->GetTile(tile_coord)->GetHeightTexture();
			
			pWaterTile water_tile = MAKE_NEW(WaterTile);
			water_tile->Init(terrain_height_texture, water_height_texture, 
				mWaterHeightField->GetTileScale().x / water_height_texture->GetDimensions().x, // pixelscale assumes square pixels
				mWaterHeightField->GetTileScale().z);

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

	mWaterHeightField = nullptr;
	mTerrainHeightField = nullptr;
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