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
	void				Init(int2 inNumTiles, int2 inTileResolution, Format inFormat = Format::FORMAT_R16G16B16A16_FLOAT, LayerType inType = LAYER_OTHER);
	void				Init(int2 inNumTiles, apRenderTarget inTargets, LayerType inType = LAYER_OTHER);
	void				CleanUp();

	int2				GetNumTiles()				{ return mNumTiles; }
	int2				GetTileResolution()			{ return mTileResolution; }
	virtual LayerType	GetType()					{ return mType; }

	
	virtual void Update() 
	{ 
		for (int y = 0; y < GetNumTiles().y; y++)
		{
			for (int x = 0; x < GetNumTiles().x; x++)
			{
				int2 tile_index = int2(x, y);
				SetDirty(tile_index, false);
			}
		}
	}


	virtual bool GetDirty(int2 inTileIndex)
	{
		assert(inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y);
		return mIsDirty[inTileIndex.y * mNumTiles.x + inTileIndex.x];
	}

	
	void SetDirty(int2 inTileIndex, bool inDirty = true)
	{ 
		assert(inTileIndex.x >= 0 && inTileIndex.x < mNumTiles.x && inTileIndex.y >= 0 && inTileIndex.y < mNumTiles.y);
		mIsDirty[inTileIndex.y * mNumTiles.x + inTileIndex.x] = inDirty; 
	}


	void SetAllDirty(bool inDirty = true)
	{
		for (int y = 0; y < GetNumTiles().y; y++)
		{
			for (int x = 0; x < GetNumTiles().x; x++)
			{
				mIsDirty[y * mNumTiles.x + x] = inDirty;
			}
		}
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

protected:
	int2 mNumTiles;
	int2 mTileResolution;
	pRenderTarget* mRenderTargets;
	LayerType mType;
	bool* mIsDirty;

private:
	bool mOwnsData;
};

