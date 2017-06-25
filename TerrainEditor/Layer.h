#pragma once
#include "Gaag.h"

REGISTERCLASS(Layer);


enum LayerType
{
	LAYER_HEIGHT,
	LAYER_NORMAL,
	LAYER_STACK,
	LAYER_OTHER
};



class Layer
{
public:
	Layer() {};
	~Layer() {};
	void Init(int2 inNumTiles, int2 inTileResolution, Format inFormat = Format::FORMAT_R16G16B16A16_FLOAT, LayerType inType = LAYER_OTHER);
	void Init(int2 inNumTiles, apRenderTarget inTargets, LayerType inType = LAYER_OTHER);
	void CleanUp();

	int2			GetNumTiles()				{ return mNumTiles; }
	int2			GetTileResolution()			{ return mTileResolution; }

	
	LayerType GetType() { return mType; }


	void SetDirty(int2 inTileIndex, bool inDirty = true)	
	{ 
		assert(inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y);
		mIsDirty[inTileIndex.y * mNumTiles.x + inTileIndex.x] = inDirty; 
	}


	bool GetDirty(int2 inTileIndex)	
	{
		assert(inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y);
		return mIsDirty[inTileIndex.y * mNumTiles.x + inTileIndex.x]; 
	}


	pRenderTarget GetTileRenderTarget(const int2& inTileIndex)
	{
		if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y)
			return mRenderTargets[inTileIndex.y * mNumTiles.x + inTileIndex.x];
		else
			return nullptr;
	}


	pTexture GetTileTexture(const int2& inTileIndex)
	{
		if (inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y)
			return GetTileRenderTarget(inTileIndex)->GetTexture();
		else
			return nullptr;
	}


private:
	int2 mNumTiles;
	int2 mTileResolution;
	pRenderTarget* mRenderTargets;
	LayerType mType;
	bool* mIsDirty;
	bool mOwnsData;
};

