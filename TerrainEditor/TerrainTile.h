#pragma once
#include "Gaag.h"

PREDEFINE(TerrainTile, pTerrainTile);

class TerrainTile : public DrawableObject
{
public:
	TerrainTile() : DrawableObject() { mInitialized = false; }
	~TerrainTile() {};

	void Init(float3 inPosition, float3 inScale, int2 inNumSegments, pMaterial inMaterial);
	void CleanUp();

	pTexture		GetTexture()		{ return mHeightMapTexture; }
	pRenderTarget	GetRenderTarget()	{ return mHeightMapRenderTarget; }
	float2			GetPixelsPerMeter()	{ return mPixelsPerMeter; }
	float			GetHeightScale()	{ return mHeightScale; }
	int2			GetSegments()		{ return mNumSegments; }

	void PrepareToDraw() override;

private:
	struct ConstantBufferData
	{
		unsigned int widthSegments;
		unsigned int heightSegments;
		float heightScale;
		float bogus_padding;
	};

	float2				mPixelsPerMeter;
	float				mHeightScale;
	int2				mNumSegments;
	ConstantBufferData	mConstantBufferData;
	pConstantBuffer		mConstantBuffer			= nullptr;
	pRenderTarget		mHeightMapRenderTarget	= nullptr;
	pTexture			mHeightMapTexture		= nullptr;
	bool				mInitialized			= false;
};

