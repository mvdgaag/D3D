#include "Layer.h"


void Layer::Init(int2 inNumTiles, int2 inTileResolution, Format inFormat, LayerType inType)
{
	CleanUp();

	mNumTiles = inNumTiles;
	mTileResolution = inTileResolution;
	mOwnsData = true;
	mType = inType;

	mIsDirty = new bool[mNumTiles.x * mNumTiles.y];
	mRenderTargets = new pRenderTarget[mNumTiles.x * mNumTiles.y];
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int idx = y * mNumTiles.x + x;
			mIsDirty[idx] = true;
			mRenderTargets[idx] = theResourceFactory.MakeRenderTarget(inTileResolution, 1, inFormat);
		}
	}
}


void Layer::Init(int2 inNumTiles, apRenderTarget inTargets, LayerType inType)
{
	assert(inTargets.size() == inNumTiles.x * inNumTiles.y);
	
	mNumTiles = inNumTiles;
	mTileResolution = inTargets[0]->GetDimensions();
	Format format = inTargets[0]->GetTexture()->GetFormat();
	mOwnsData = false;
	mType = inType;

	mIsDirty = new bool[mNumTiles.x * mNumTiles.y];
	mRenderTargets = new pRenderTarget[mNumTiles.x * mNumTiles.y];
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int idx = x * mNumTiles.y + y;
			int idx2 = y * mNumTiles.x + x;// TODO: this has X and Y flipped. Make consistent?
			mIsDirty[idx] = true;
			pRenderTarget target = inTargets[idx];
			assert(target->GetDimensions() == mTileResolution);
			assert(target->GetTexture()->GetFormat() == format);
			mRenderTargets[idx] = inTargets[idx2];
		}
	}
}


void Layer::CleanUp()
{
	if (mOwnsData)
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
	}
	delete[] mIsDirty;
	mRenderTargets = nullptr;
}
