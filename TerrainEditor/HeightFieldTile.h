#pragma once
#include "Gaag.h"

PREDEFINE(HeightFieldTile, pHeightFieldTile);

class HeightFieldTile : public DrawableObject
{
public:
	HeightFieldTile() : DrawableObject() { mInitialized = false; }
	~HeightFieldTile() {};

	void Init(float3 inPosition, float3 inScale, int2 inNumSegments, pMaterial inMaterial, pTexture inHeightTexture);
	void PrepareToDraw() override;
	void FinalizeAfterDraw() override;
	void CleanUp();

	void			SetTexture(pTexture inTexture);

	pTexture		GetTexture()					{ return mHeightMapTexture; }
	pRenderTarget	GetRenderTarget()				{ return mHeightMapRenderTarget; }
	float2			GetPixelsPerMeter()				{ return mPixelsPerMeter; }
	float			GetHeightScale()				{ return mHeightScale; }
	int2			GetSegments()					{ return mNumSegments; }

private:
	struct ConstantBufferData
	{
		float4 scale;
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

