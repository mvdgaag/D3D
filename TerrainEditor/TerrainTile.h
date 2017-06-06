#pragma once
#include "Gaag.h"

REGISTERCLASS(TerrainTile);

class TerrainTile : public MeshObject
{
public:
	TerrainTile() : MeshObject() { mInitialized = false; }
	~TerrainTile() {};

	void Init(float3 inPosition, float3 inScale, int2 inNumSegments, pMaterial inMaterial, pMaterial inShadowMaterial, pTexture inHeightTexture);
	void CleanUp();
	void UpdateNormals();
	void PrepareToDraw() override;
	void FinalizeAfterDraw() override;
	void PrepareToDrawShadow() override;
	void FinalizeAfterDrawShadow() override;

	pTexture		GetHeightTexture()				{ return mHeightMapTexture; }
	pTexture		GetNormalTexture()				{ return mNormalTexture; }
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
	pRenderTarget		mNormalRenderTarget		= nullptr;
	pTexture			mNormalTexture			= nullptr;
	pComputeShader		mUpdateNormalShader		= nullptr;
	pMaterial			mCustomShadowMaterial	= nullptr;
	bool				mInitialized			= false;
};

