#pragma once
#include "Gaag.h"

REGISTERCLASS(TerrainTile);

class TerrainTile : public MeshObject
{
public:
	TerrainTile() : MeshObject() { mInitialized = false; }
	~TerrainTile() {};

	void Init(float3 inPosition, float3 inScale, int2 inNumVertices, pMaterial inMaterial, pMaterial inShadowMaterial, pTexture inHeightTexture, pTexture inNormalTexture);
	void CleanUp();
	void UpdateNormals(apTexture inNeighborHeights);
	void PrepareToDraw() override;
	void FinalizeAfterDraw() override;
	void PrepareToDrawShadow() override;
	void FinalizeAfterDrawShadow() override;

	pTexture		GetHeightTexture()				{ return mHeightMapTexture; }
	pTexture		GetNormalTexture()				{ return mNormalTexture; }
	pRenderTarget	GetRenderTarget()				{ return mHeightMapRenderTarget; }
	float2			GetPixelsPerMeter()				{ return mPixelsPerMeter; }
	float			GetHeightScale()				{ return mHeightScale; }
	int2			GetSegments()					{ return mNumVertices; }

private:
	struct ConstantBufferData
	{
		float4	scale;
		int4	textureInfo;
	};

	float2				mPixelsPerMeter;
	float				mHeightScale;
	int2				mNumVertices;
	ConstantBufferData	mConstantBufferData;
	pConstantBuffer		mConstantBuffer			= nullptr;
	pRenderTarget		mHeightMapRenderTarget	= nullptr;
	pTexture			mHeightMapTexture		= nullptr;
	pRenderTarget		mNormalRenderTarget		= nullptr;
	pTexture			mNormalTexture			= nullptr;
	pMaterial			mCustomShadowMaterial	= nullptr;
	bool				mInitialized			= false;

	static				pComputeShader sUpdateNormalShader;
};

