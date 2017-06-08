#pragma once
#include "Gaag.h"

REGISTERCLASS(Layer);


class Layer
{
public:
	Layer() {};
	~Layer() {};
	void Init(int2 inNumTiles, int2 inTileResolution, Format inFormat = Format::FORMAT_R16G16B16A16_FLOAT);
	void Init(int2 inNumTiles, apRenderTarget inTargets);
	void CleanUp();

	int2			GetNumTiles()				{ return mNumTiles; }
	int2			GetTileResolution()			{ return mTileResolution; }


	pRenderTarget GetTileRenderTarget(const int2& inTileIndex)
	{
		if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y)
			return mRenderTargets[inTileIndex.y * mNumTiles.x + inTileIndex.x];
		else
			return nullptr;
	}


	pTexture GetTileTexture(const int2& inTileIndex)
	{
		return GetTileRenderTarget(inTileIndex)->GetTexture();
	}


private:
	int2 mNumTiles;
	int2 mTileResolution;
	pRenderTarget* mRenderTargets;
	bool mOwnsData;
};

