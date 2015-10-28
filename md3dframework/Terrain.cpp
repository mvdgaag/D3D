#include "Terrain.h"
#include "Framework.h"
#include "TerrainTile.h"
#include "Material.h"
#include "Texture.h"


void Terrain::Init(int inWidth, int inHeight, int inTileWidth, int inTileHeight, pMaterial inMaterial)
{
	CleanUp();
	mWidth = inWidth;
	mHeight = inHeight;
	mTiles = new pTerrainTile[mWidth * mHeight];
	for (int y = 0; y < mHeight; y++)
	{
		for (int x = 0; x < mWidth; x++)
		{
			int idx = y * mWidth + x;
			
			float3 tile_scale(1.0, 1.0, 1.0);
			
			float3 tile_pos;
			tile_pos.x = (float(x) - float(mWidth) / 2.0) * tile_scale.x;
			tile_pos.y = 0.0;
			tile_pos.z = (float(y) - float(mHeight) / 2.0) * tile_scale.y;

			pTexture height_map = std::make_shared<Texture>();
			height_map->Init(inTileWidth, inTileHeight, 1, 41); // 41 = DXGI_FORMAT_R32_FLOAT
			
			mTiles[idx] = std::make_shared<TerrainTile>();
			mTiles[idx]->Init(tile_pos, tile_scale, height_map, inTileWidth, inTileHeight, inMaterial);
			
			theFramework.RegisterObject(mTiles[idx]);
		}
	}
}


void Terrain::CleanUp()
{
	for (int y = 0; y < mHeight; y++)
	{
		for (int x = 0; x < mWidth; x++)
		{
			int idx = y * mWidth + x;
			mTiles[idx] = nullptr;
		}
	}
	delete[] mTiles;
	mTiles = nullptr;
}
