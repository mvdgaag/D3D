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
	mWaterHeightTarget = MAKE_NEW(RenderTarget);
	mWaterHeightTarget->Init(inWaterHeightTexture);

	// collect data for creating the other textures/rendertargets
	int width = inWaterHeightTexture->GetWidth();
	int height = inWaterHeightTexture->GetHeight();
	Format format = inWaterHeightTexture->GetFormat();
	BindFlag bind_flags = required_flags;
	CPUAccessFlag cpu_access_flags = CPUAccessFlag::CPU_ACCESS_DEFAULT;

	// flux has four components, but dx11 does not allow writing both from compute
	// to they are interleaved in an R32 texture of four times the width
	pTexture flux_texture = MAKE_NEW(Texture);
	flux_texture->Init(width * 4, height, 1, FORMAT_R32_FLOAT, 0, bind_flags, cpu_access_flags);
	mFluxRenderTarget = MAKE_NEW(RenderTarget);
	mFluxRenderTarget->Init(flux_texture);

	// create the water height rendertarget for drawing (this will total height plus water)
	mWaterDepthTarget = MAKE_NEW(RenderTarget);
	mWaterDepthTarget->Init(width, height, 1, FORMAT_R32_FLOAT);

	// create the compute shaders for the sim
	mUpdateFluxShader = MAKE_NEW(ComputeShader);
	mUpdateFluxShader->InitFromFile("Shaders/UpdateFlux.hlsl");

	mUpdateFluxBorderShader = MAKE_NEW(ComputeShader);
	mUpdateFluxBorderShader->InitFromFile("Shaders/UpdateFluxBorder.hlsl");

	mUpdateWaterShader = MAKE_NEW(ComputeShader);
	mUpdateWaterShader->InitFromFile("Shaders/UpdateWater.hlsl");

	mUpdateWaterBorderShader = MAKE_NEW(ComputeShader);
	mUpdateWaterBorderShader->InitFromFile("Shaders/UpdateWaterBorder.hlsl");

	mTimeStep = 0.01;
	mFluidity = 10.0;
	mGravity = 9.81;
	mPixelScale = inPixelScale;
	mHeightScale = inHeightScale;
	mFriction = 0.9;

	mFluxConstantBuffer = MAKE_NEW(ConstantBuffer);
	mFluxConstantBuffer->Init(sizeof(float4));

	mWaterConstantBuffer = MAKE_NEW(ConstantBuffer);
	mWaterConstantBuffer->Init(sizeof(float4));
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
	int2 num_threads = (mFluxRenderTarget->GetTexture()->GetResolution() + 7) / 8;
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
	if (inNorthNeighbor) 
		theRenderContext.CSSetTexture(inNorthNeighbor->GetWaterHeightTexture(), 2);
	if (inEastNeighbor) 
		theRenderContext.CSSetTexture(inEastNeighbor->GetWaterHeightTexture(), 3);
	if (inSouthNeighbor)
		theRenderContext.CSSetTexture(inSouthNeighbor->GetFluxTexture(), 4);
	if (inWestNeighbor)
		theRenderContext.CSSetTexture(inWestNeighbor->GetFluxTexture(), 5);
	theRenderContext.CSSetConstantBuffer(mFluxConstantBuffer, 0);
	num_threads = (mFluxRenderTarget->GetTexture()->GetResolution() + 31) / 32;
	theRenderContext.Dispatch(num_threads.x, 0, 1); // assumes square texture
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
	int2 num_threads = (mWaterDepthTarget->GetTexture()->GetResolution() + 7) / 8;
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
	if (inNorthNeighbor) 
		theRenderContext.CSSetTexture(inNorthNeighbor->GetFluxTexture(), 2);
	if (inEastNeighbor) 
		theRenderContext.CSSetTexture(inEastNeighbor->GetFluxTexture(), 3);
	if (inSouthNeighbor)
	{
		theRenderContext.CSSetTexture(inSouthNeighbor->GetWaterDepthTexture(), 4);
		theRenderContext.CSSetTexture(inSouthNeighbor->GetWaterHeightTexture(), 6);
	}
	if (inWestNeighbor)
	{
		theRenderContext.CSSetTexture(inWestNeighbor->GetWaterDepthTexture(), 5);
		theRenderContext.CSSetTexture(inWestNeighbor->GetWaterHeightTexture(), 7);
	}
	theRenderContext.CSSetConstantBuffer(mWaterConstantBuffer, 0);
	num_threads = (mWaterDepthTarget->GetTexture()->GetResolution() + 31) / 32;
	theRenderContext.Dispatch(num_threads.x, 1, 1); // assumes square texture
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 1);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 1);
	theRenderContext.CSSetTexture(NULL, 2);
	theRenderContext.CSSetTexture(NULL, 3);
	theRenderContext.CSSetTexture(NULL, 4);
	theRenderContext.CSSetTexture(NULL, 5);
	theRenderContext.CSSetTexture(NULL, 6);
	theRenderContext.CSSetTexture(NULL, 7);
	theRenderContext.CSSetShader(NULL);
}