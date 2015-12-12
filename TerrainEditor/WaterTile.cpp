#include "WaterTile.h"


void WaterTile::Init(pTexture inTerrainHeightTexture, pTexture inWaterHeightTexture, float inPixelScale, float inHeightScale)
{
	CleanUp();

	// assert for assumptions
	BindFlag required_flags = BindFlag::BIND_SHADER_RESOURCE | BindFlag::BIND_UNORDERED_ACCESS | BindFlag::BIND_RENDER_TARGET;
	assert((inWaterHeightTexture->GetBindFlags() & required_flags) == required_flags);
	assert(inWaterHeightTexture->GetFormat() == Format::FORMAT_R32_FLOAT);
	assert(inTerrainHeightTexture != nullptr);

	mTerrainHeightTexture = inTerrainHeightTexture;

	// create waterheight render target
	mWaterHeightTarget = theResourceFactory.MakeRenderTarget(inWaterHeightTexture);

	// collect data for creating the other textures/rendertargets
	mResolution.x = inWaterHeightTexture->GetWidth();
	mResolution.y = inWaterHeightTexture->GetHeight();
	Format format = inWaterHeightTexture->GetFormat();
	BindFlag bind_flags = required_flags;

	// flux has four components, but dx11 does not allow writing both from compute
	// to they are interleaved in an R32 texture of four times the width
	pTexture flux_texture = theResourceFactory.MakeTexture(int2(mResolution.x * 4, mResolution.y), 1, FORMAT_R32_FLOAT, bind_flags);
	mFluxRenderTarget = theResourceFactory.MakeRenderTarget(flux_texture);

	// create the water height rendertarget for drawing (this will total height plus water)
	mWaterDepthTarget = theResourceFactory.MakeRenderTarget(int2(mResolution.x, mResolution.y), 1, FORMAT_R32_FLOAT);

	// create the compute shaders for the sim
	// TODO get instead of load. Load should be unique in init somewhere
	mUpdateFluxShader = theResourceFactory.LoadComputeShader("Shaders/UpdateFlux.hlsl");
	mUpdateFluxBorderShader = theResourceFactory.LoadComputeShader("Shaders/UpdateFluxBorder.hlsl");
	mUpdateWaterShader = theResourceFactory.LoadComputeShader("Shaders/UpdateWater.hlsl");
	mUpdateWaterBorderShader = theResourceFactory.LoadComputeShader("Shaders/UpdateWaterBorder.hlsl");

	mTimeStep = 0.01;
	mFluidity = 100.0;
	mGravity = 9.81;
	mPixelScale = inPixelScale;
	mHeightScale = inHeightScale;
	mFriction = 0.1;

	mFluxConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(float4));
	mWaterConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(float4));

	TextureUtil::TextureSet(mWaterDepthTarget->GetTexture(), float4(0.5, 0.5, 0.5, 0.5));
}


void WaterTile::CleanUp()
{
	mFluxConstantBuffer = nullptr;
	mWaterConstantBuffer = nullptr;
	mUpdateFluxShader = nullptr;
	mUpdateFluxBorderShader = nullptr;
	mUpdateWaterShader = nullptr;
	mUpdateWaterBorderShader = nullptr;
	mTerrainHeightTexture = nullptr;
	mWaterDepthTarget = nullptr;
	mWaterHeightTarget = nullptr;
	mFluxRenderTarget = nullptr;
}


void WaterTile::UpdateFlux(pWaterTile inNorthNeighbor, pWaterTile inEastNeighbor, pWaterTile inSouthNeighbor, pWaterTile inWestNeighbor)
{
	assert(inNorthNeighbor && inEastNeighbor && inSouthNeighbor && inWestNeighbor);

	float4 constant_data;
	constant_data.x = mTimeStep * mFluidity * mPixelScale * mGravity * mHeightScale;
	constant_data.y = mFriction;
	constant_data.z = mTimeStep,
	constant_data.w = 1.0 / (mHeightScale * mPixelScale * mPixelScale); // pixel_value / m^3
	theRenderContext.UpdateSubResource(*mFluxConstantBuffer, &constant_data);

	// simulate center area
	theRenderContext.CSSetShader(mUpdateFluxShader);
	theRenderContext.CSSetRWTexture(mFluxRenderTarget, 0);
	theRenderContext.CSSetTexture(mWaterHeightTarget->GetTexture(), 0);
	theRenderContext.CSSetTexture(mWaterDepthTarget->GetTexture(), 1);
	theRenderContext.CSSetConstantBuffer(mFluxConstantBuffer, 0);
	int2 num_threads = (mResolution + 7) / 8;
	theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 1);
	theRenderContext.CSSetShader(NULL);

	// copy and simulate borders
	theRenderContext.CSSetShader(mUpdateFluxBorderShader);
	theRenderContext.CSSetRWTexture(mFluxRenderTarget, 0);
	theRenderContext.CSSetTexture(mWaterHeightTarget->GetTexture(), 0);
	theRenderContext.CSSetTexture(mWaterDepthTarget->GetTexture(), 1);
	theRenderContext.CSSetTexture(inNorthNeighbor->GetWaterHeightTexture(), 2);
	theRenderContext.CSSetTexture(inEastNeighbor->GetWaterHeightTexture(), 3);
	theRenderContext.CSSetTexture(inSouthNeighbor->GetWaterHeightTexture(), 4);
	theRenderContext.CSSetTexture(inWestNeighbor->GetWaterHeightTexture(), 5);
	theRenderContext.CSSetConstantBuffer(mFluxConstantBuffer, 0);
	num_threads = (mResolution + 31) / 32;
	theRenderContext.Dispatch(num_threads.x, 4, 1); // assumes square texture
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 1);
	theRenderContext.CSSetTexture(NULL, 2);
	theRenderContext.CSSetTexture(NULL, 3);
	theRenderContext.CSSetTexture(NULL, 4);
	theRenderContext.CSSetTexture(NULL, 5);
	theRenderContext.CSSetShader(NULL);
}


void WaterTile::UpdateWater(pWaterTile inNorthNeighbor, pWaterTile inEastNeighbor, pWaterTile inSouthNeighbor, pWaterTile inWestNeighbor)
{
	assert(inNorthNeighbor && inEastNeighbor && inSouthNeighbor && inWestNeighbor);

	float4 constant_data;
	constant_data.x = mHeightScale;
	constant_data.y = mPixelScale * mPixelScale;
	constant_data.z = mTimeStep,
	constant_data.w = 1.0 / (mHeightScale * mPixelScale * mPixelScale); // pixel_value / m^3
	theRenderContext.UpdateSubResource(*mWaterConstantBuffer, &constant_data);

	// simulate center area
	theRenderContext.CSSetShader(mUpdateWaterShader);
	theRenderContext.CSSetRWTexture(mWaterDepthTarget, 0);
	theRenderContext.CSSetRWTexture(mWaterHeightTarget, 1);
	theRenderContext.CSSetTexture(mTerrainHeightTexture, 0);
	theRenderContext.CSSetTexture(mFluxRenderTarget->GetTexture(), 1);
	theRenderContext.CSSetConstantBuffer(mWaterConstantBuffer, 0);
	int2 num_threads = (mResolution + 7) / 8;
	theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 1);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 1);
	theRenderContext.CSSetShader(NULL);

	// copy and simulate borders
	theRenderContext.CSSetShader(mUpdateWaterBorderShader);
	theRenderContext.CSSetRWTexture(mWaterDepthTarget, 0);
	theRenderContext.CSSetRWTexture(mWaterHeightTarget, 1);
	
	theRenderContext.CSSetTexture(mTerrainHeightTexture, 0);
	theRenderContext.CSSetTexture(mFluxRenderTarget->GetTexture(), 1);
	theRenderContext.CSSetTexture(inNorthNeighbor->GetFluxTexture(), 2);
	theRenderContext.CSSetTexture(inEastNeighbor->GetFluxTexture(), 3);
	theRenderContext.CSSetTexture(inSouthNeighbor->GetFluxTexture(), 4);
	theRenderContext.CSSetTexture(inWestNeighbor->GetFluxTexture(), 5);
	theRenderContext.CSSetConstantBuffer(mWaterConstantBuffer, 0);
	num_threads = (mResolution + 31) / 32;
	theRenderContext.Dispatch(num_threads.x, 4, 1); // assumes square texture
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 1);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 1);
	theRenderContext.CSSetTexture(NULL, 2);
	theRenderContext.CSSetTexture(NULL, 3);
	theRenderContext.CSSetTexture(NULL, 4);
	theRenderContext.CSSetTexture(NULL, 5);
	theRenderContext.CSSetShader(NULL);
}