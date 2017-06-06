#include "Layer.h"


void Layer::Init(int2 inNumTiles, int2 inTileResolution, Format inFormat)
{
	CleanUp();

	mNumTiles = inNumTiles;
	mTileResolution = inTileResolution;

	mRenderTargets = new pRenderTarget[mNumTiles.x * mNumTiles.y];
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int idx = y * mNumTiles.x + x;
			mRenderTargets[idx] = theResourceFactory.MakeRenderTarget(inTileResolution, 1, inFormat);
		}
	}
}


void Layer::CleanUp()
{
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int idx = y * mNumTiles.x + x;
			mRenderTargets[idx]->CleanUp();
			mRenderTargets[idx] = nullptr;
		}
	}
	delete[] mRenderTargets;
	mRenderTargets = nullptr;
}
