#include "Terrain.h"
#include "TerrainTile.h"


void Terrain::Init(int2 inNumTiles, int2 inTileSegments, float3 inTileScale, pMaterial inMaterial, pMaterial inShadowMaterial)
{
	CleanUp();
	
	mNumTiles = inNumTiles;
	mTileSegments = inTileSegments;
	mTileScale = inTileScale;

	pLayer height_layer = new Layer();
	height_layer->Init(inNumTiles, int2(mTileSegments.x + 1, mTileSegments.y + 1), FORMAT_R32_FLOAT);
	mLayers.push_back(height_layer);

	mTiles = new pTerrainTile[mNumTiles.x * mNumTiles.y];
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int idx = y * mNumTiles.x + x;
			
			float3 tile_pos;
			tile_pos.x = (float(x) - float(mNumTiles.x - 1) / 2.0) * mTileScale.x;
			tile_pos.y = 0.0;
			tile_pos.z = (float(y) - float(mNumTiles.y - 1) / 2.0) * mTileScale.y;
			
			mTiles[idx] = MAKE_NEW(TerrainTile);
			pMaterial material = theResourceFactory.CloneMaterial(inMaterial);
			material->SetDiffuseValue(float4(1, 0, 0, 0));
			mTiles[idx]->Init(tile_pos, mTileScale, mTileSegments, material, inShadowMaterial, height_layer->GetTileTexture(int2(x, y)));
			
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

	for each (pLayer layer in mLayers)
		layer->CleanUp();
	mLayers.clear();
}
