#include "Water.h"
#include "Terrain.h"
#include "WaterTile.h"


void Water::Init(pTerrain inTerrain, pMaterial inMaterial)
{
	assert(inTerrain != nullptr);
	mTerrain = inTerrain;

	mHeightField = MAKE_NEW(Terrain);
	mHeightField->Init(mTerrain->GetNumTiles(), mTerrain->GetTileSegments(), mTerrain->GetTileScale(), inMaterial);

	mNumTiles = mHeightField->GetNumTiles();
	mWaterTiles = new pWaterTile*[mNumTiles.x];
	for (int x = 0; x < mNumTiles.x; x++)
	{
		mWaterTiles[x] = new pWaterTile[mNumTiles.y];
		for (int y = 0; y < mNumTiles.y; y++)
		{
			int2 tile_coord(x, y);
			pTexture terrain_height_texture = mTerrain->GetTile(tile_coord)->GetTexture();
			pTexture water_height_texture = mHeightField->GetTile(tile_coord)->GetTexture();
			
			pWaterTile water_tile = MAKE_NEW(WaterTile);
			water_tile->Init(terrain_height_texture, water_height_texture, 
				mHeightField->GetTileScale().x / water_height_texture->GetWidth(), // pixelscale assumes square pixels
				mHeightField->GetTileScale().z);

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

	mHeightField = nullptr;
	mTerrain = nullptr;
}


void Water::Update(float inTimeStep)
{
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			mWaterTiles[x][y]->Update(inTimeStep);
		}
	}
}