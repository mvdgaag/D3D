#pragma once
#include "Gaag.h"

PREDEFINE(WaterTile, pWaterTile);

class WaterTile
{
public:
	WaterTile() {}
	~WaterTile() {}

	void Init(pTexture inTerrainHeightTexture, pTexture inWaterHeightTexture, float inPixelScale, float inHeightScale);
	void Update(float inTimeStep);
	pTexture GetWaterHeightTexture() { return mWaterHeightTarget->GetTexture(); }
	pTexture GetWaterDepthTexture() { return mWaterDepthTarget->GetTexture(); }
	pTexture GetFluxTexture() { return mFluxRenderTarget->GetTexture(); }

private:
	void UpdateFlux();
	void UpdateWater();

	pConstantBuffer	mFluxConstantBuffer;
	pConstantBuffer	mWaterConstantBuffer;
	float mFluidity;			// unitless constant, low means high viscocity
	float mGravity;				// meters / second^2
	float mPixelScale;			// meters / pixel
	float mHeightScale;			// meters / pixelvalue
	float mFriction;			// unitless
	float mMaxFrictionDepth;	// meters depth where fluidity is constant (0..mMaxfrictionDepth is mapped to 0..1 in dFlux)

	pComputeShader	mUpdateFluxShader;
	pComputeShader	mUpdateWaterShader;

	pTexture		mTerrainHeightTexture;
	pRenderTarget	mWaterDepthTarget;
	pRenderTarget	mWaterHeightTarget;

	pRenderTarget	mFluxRenderTarget;
};

