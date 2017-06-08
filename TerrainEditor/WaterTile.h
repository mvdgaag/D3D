#pragma once
#include "Gaag.h"

REGISTERCLASS(WaterTile);

class WaterTile
{
public:
	WaterTile() {}
	~WaterTile() { CleanUp(); }

	void Init(pTexture inTerrainHeightTexture, pTexture inWaterHeightTexture, float inPixelScale, float inHeightScale);
	void CleanUp();

	void UpdateFlux(pWaterTile inNorthNeighbor, pWaterTile inEastNeighbor, pWaterTile inSouthNeighbor, pWaterTile inWestNeighbor);
	void UpdateWater(pWaterTile inNorthNeighbor, pWaterTile inEastNeighbor, pWaterTile inSouthNeighbor, pWaterTile inWestNeighbor);

	pTexture GetWaterHeightTexture() { return mWaterHeightTarget->GetTexture(); }
	pTexture GetWaterDepthTexture() { return mWaterDepthTarget->GetTexture(); }
	pTexture GetFluxTexture() { return mFluxRenderTarget->GetTexture(); }

	pRenderTarget GetWaterHeightRenderTarget() { return mWaterHeightTarget; }
	pRenderTarget GetWaterDepthRenderTarget() { return mWaterDepthTarget; }
	pRenderTarget GetFluxRenderTarget() { return mFluxRenderTarget; }

private:

	pConstantBuffer	mFluxConstantBuffer;
	pConstantBuffer	mWaterConstantBuffer;
	float mTimeStep;			// seconds
	float mFluidity;			// unitless constant, low means high viscocity
	float mGravity;				// meters / second^2
	float mPixelScale;			// meters / pixel
	float mHeightScale;			// meters / pixelvalue
	float mFriction;			// unitless
	float mMaxFrictionDepth;	// meters depth where fluidity is constant (0..mMaxfrictionDepth is mapped to 0..1 in dFlux)
	int2 mResolution;

	pComputeShader	mUpdateFluxShader;
	pComputeShader	mUpdateFluxBorderShader;
	pComputeShader	mUpdateWaterShader;
	pComputeShader	mUpdateWaterBorderShader;

	pTexture		mTerrainHeightTexture;
	pRenderTarget	mWaterDepthTarget;
	pRenderTarget	mWaterHeightTarget;

	pRenderTarget	mFluxRenderTarget;
};

