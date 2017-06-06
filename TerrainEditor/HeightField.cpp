#include "HeightField.h"
#include "HeightFieldTile.h"


void HeightField::Init(int2 inNumTiles, int2 inTileSegments, float3 inTileScale, pMaterial inMaterial, pMaterial inShadowMaterial)
{
	CleanUp();
	
	mNumTiles = inNumTiles;
	mTileSegments = inTileSegments;
	mTileScale = inTileScale;

	mTiles = new pHeightFieldTile[mNumTiles.x * mNumTiles.y];
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int idx = y * mNumTiles.x + x;
			
			float3 tile_pos;
			tile_pos.x = (float(x) - float(mNumTiles.x - 1) / 2.0) * mTileScale.x;
			tile_pos.y = 0.0;
			tile_pos.z = (float(y) - float(mNumTiles.y - 1) / 2.0) * mTileScale.y;

			pTexture heightmap = theResourceFactory.MakeTexture(int2(mTileSegments.x + 1, mTileSegments.y + 1), 1, FORMAT_R32_FLOAT, BIND_COMPUTE_TARGET);

			mTiles[idx] = MAKE_NEW(HeightFieldTile);
			pMaterial material = theResourceFactory.CloneMaterial(inMaterial);
			material->SetDiffuseValue(float4(1, 0, 0, 0));
			mTiles[idx]->Init(tile_pos, mTileScale, mTileSegments, material, inShadowMaterial, heightmap);
			
			Gaag.RegisterObject(mTiles[idx]);
		}
	}
}


void HeightField::CleanUp()
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
