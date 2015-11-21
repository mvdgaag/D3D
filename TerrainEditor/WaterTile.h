#pragma once
#include "Gaag.h"

PREDEFINE(WaterTile, pWaterTile);

class WaterTile
{
public:
	WaterTile() {};
	~WaterTile() {};

	void Init(pTexture inTerrainHeightTexture, pTexture inWaterHeightTexture);
	void Update();
	pTexture GetWaterHeightTexture() { return mWaterHeightTarget->GetTexture(); }
	pTexture GetWaterDepthTexture() { return mWaterDepthTarget->GetTexture(); }
	pTexture GetFluxTexture() { return mFluxRenderTarget->GetTexture(); }

private:
	void UpdateFlux();
	void UpdateWater();
	pComputeShader	mUpdateFluxShader;
	pComputeShader	mUpdateWaterShader;

	pTexture		mTerrainHeightTexture;
	pRenderTarget	mWaterDepthTarget;
	pRenderTarget	mWaterHeightTarget;

	pRenderTarget	mFluxRenderTarget;
};

