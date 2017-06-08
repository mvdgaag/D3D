#include "Water.h"
#include "Terrain.h"
#include "WaterTile.h"


void Water::Init(pTerrain inTerrainTerrain, pMaterial inMaterial, pMaterial inShadowMaterial)
{
	assert(inTerrainTerrain != nullptr);
	mTerrain = inTerrainTerrain;

	mWaterSurface = MAKE_NEW(Terrain);
	mWaterSurface->Init(mTerrain->GetNumTiles(), mTerrain->GetTileSegments(), mTerrain->GetTileScale(), inMaterial, inShadowMaterial);

	mNumTiles = mWaterSurface->GetNumTiles();
	mWaterTiles = new pWaterTile*[mNumTiles.x];
	apRenderTarget water_targets;
	for (int x = 0; x < mNumTiles.x; x++)
	{
		mWaterTiles[x] = new pWaterTile[mNumTiles.y];
		for (int y = 0; y < mNumTiles.y; y++)
		{
			int2 tile_coord(x, y);
			pTexture terrain_height_texture = mTerrain->GetTile(tile_coord)->GetHeightTexture();
			pTexture water_height_texture = mWaterSurface->GetTile(tile_coord)->GetHeightTexture();
			
			pWaterTile water_tile = MAKE_NEW(WaterTile);
			water_tile->Init(terrain_height_texture, water_height_texture, 
				mWaterSurface->GetTileScale().x / water_height_texture->GetDimensions().x, // pixelscale assumes square pixels
				mWaterSurface->GetTileScale().z);

			mWaterTiles[x][y] = water_tile;
			water_targets.push_back(water_tile->GetWaterDepthRenderTarget());
		}
	}
	pLayer water_layer = new Layer();
	water_layer->Init(mWaterSurface->GetNumTiles(), water_targets);
	mWaterSurface->SetLayer(water_layer, 1);
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

	mWaterSurface = nullptr;
	mTerrain = nullptr;
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