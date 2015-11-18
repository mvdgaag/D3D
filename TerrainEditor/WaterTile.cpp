#include "WaterTile.h"


void WaterTile::Init(pTexture inTerrainHeightTexture, pTexture inWaterHeightTexture)
{
	BindFlag required_flags = BindFlag::BIND_SHADER_RESOURCE | BindFlag::BIND_UNORDERED_ACCESS | BindFlag::BIND_RENDER_TARGET;
	assert((inWaterHeightTexture->GetBindFlags() & required_flags) == required_flags);
	assert(inWaterHeightTexture->GetFormat() == Format::FORMAT_R32_FLOAT);

	mTerrainTexture = inTerrainHeightTexture;
	mWaterRenderTarget = MAKE_NEW(RenderTarget);
	mWaterRenderTarget->Init(inWaterHeightTexture);
	
	int width = inWaterHeightTexture->GetWidth();
	int height = inWaterHeightTexture->GetHeight();
	Format format = inWaterHeightTexture->GetFormat();
	BindFlag bind_flags = required_flags;
	CPUAccessFlag cpu_access_flags = CPUAccessFlag::CPU_ACCESS_DEFAULT;

	pTexture flux_texture = MAKE_NEW(Texture);
	flux_texture->Init(width * 2, height, 1, FORMAT_R32_FLOAT, 0, bind_flags, cpu_access_flags);
	mFluxRenderTarget = MAKE_NEW(RenderTarget);
	mFluxRenderTarget->Init(flux_texture);

	mUpdateFluxShader = MAKE_NEW(ComputeShader);
	mUpdateFluxShader->InitFromFile("Shaders/UpdateFlux.hlsl");

	mUpdateWaterShader = MAKE_NEW(ComputeShader);
	mUpdateWaterShader->InitFromFile("Shaders/UpdateWater.hlsl");
}


void WaterTile::Update()
{
	UpdateFlux();
	UpdateWater();
}


void WaterTile::UpdateFlux()
{
	// update flux
	theRenderContext.CSSetShader(mUpdateFluxShader);
	theRenderContext.CSSetRWTexture(mFluxRenderTarget, 0);
	theRenderContext.CSSetTexture(mTerrainTexture, 0);
	theRenderContext.CSSetTexture(mWaterRenderTarget->GetTexture(), 1);

	int2 num_threads = (mFluxRenderTarget->GetTexture()->GetResolution() + 7) / 8;
	theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 1);
	theRenderContext.CSSetShader(NULL);
}


void WaterTile::UpdateWater()
{
	// update water
	theRenderContext.CSSetShader(mUpdateWaterShader);
	theRenderContext.CSSetRWTexture(mWaterRenderTarget, 0);
	theRenderContext.CSSetTexture(mTerrainTexture, 0);
	theRenderContext.CSSetTexture(mFluxRenderTarget->GetTexture(), 1);

	int2 num_threads = (mWaterRenderTarget->GetTexture()->GetResolution() + 7) / 8;
	theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 1);
	theRenderContext.CSSetShader(NULL);
}