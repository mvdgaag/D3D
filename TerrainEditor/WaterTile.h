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
	pTexture GetWaterTexture() { return mWaterRenderTarget->GetTexture(); }
	pTexture GetFluxTexture() { return mFluxRenderTarget->GetTexture(); }

private:
	void UpdateFlux();
	void UpdateWater();
	pComputeShader	mUpdateFluxShader;
	pComputeShader	mUpdateWaterShader;
	pTexture		mTerrainTexture;
	pRenderTarget	mWaterRenderTarget;
	pRenderTarget	mFluxRenderTarget;
};

