#include "SimpleHeightFlow.h"
#include "Terrain.h"
#include "Layer.h"
#include "ConstantBuffer.h"
#include "ComputeShader.h"



void SimpleHeightFlow::Init(pTerrain inTerrain)
{
	CleanUp();

	assert(inTerrain != nullptr);
	
	mResolution = inTerrain->GetTileResolution();
	mScale = inTerrain->GetTileScale();

	mFluidity = 10.0f;
	mFriction = 0.99f;
	mTalusAngle = 0.0f * mScale.z / (mScale.x / mResolution.x); // assume square pixels!

	mLayer = new Layer();
	mLayer->Init(inTerrain->GetNumTiles(), mResolution, Format::FORMAT_R16G16B16A16_FLOAT);
	mLayerIndex = inTerrain->SetLayer(mLayer);

	mHistoryLayer = new Layer();
	mHistoryLayer->Init(inTerrain->GetNumTiles(), mResolution, Format::FORMAT_R16G16B16A16_FLOAT);
	mHistoryLayerIndex = inTerrain->SetLayer(mHistoryLayer);

	mUpdateShader = theResourceFactory.LoadComputeShader("Shaders/UpdateSimpleHeightFlow.hlsl");
	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(mConstantBufferData));

	for (int y = 0; y < mLayer->GetNumTiles().y; y++)
	{
		for (int x = 0; x < mLayer->GetNumTiles().x; x++)
		{
			TextureUtil::TextureSet(mLayer->GetTileTexture(int2(x, y)), float4(0.0, 0.0, 0.0, 0.0));
		}
	}

	mInitialized = true;
}


void SimpleHeightFlow::CleanUp()
{
	delete mLayer;
	mLayer = nullptr;

	// TODO: tell resource factory to free these resources?
	mConstantBuffer = nullptr;
	mUpdateShader = nullptr;

	mInitialized = false;
}


void SimpleHeightFlow::Update(float inTimeStep)
{
	// DEVHACK
	//assert(inTimeStep * mFluidity <= 1.0);

	if (!mInitialized)
		return;

	mConstantBufferData.params = float4(min(1.0, mFluidity * inTimeStep), mFriction, mTalusAngle, 0.0);
	mConstantBufferData.resolution = float4(float2(mLayer->GetTileResolution()), float2(1.0, 1.0) / float2(mLayer->GetTileResolution()));
	mConstantBufferData.scale = float4(mScale, 0.0);
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);

	for (int y = 0; y < mLayer->GetNumTiles().y; y++)
	{
		for (int x = 0; x < mLayer->GetNumTiles().x; x++)
		{
			theRenderContext.CSSetShader(mUpdateShader);
			theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);
			int2 num_threads = (mResolution + 7) / 8;

			// ping
			theRenderContext.CSSetRWTexture(mHistoryLayer->GetTileRenderTarget(int2(x, y)), 0, 0);
			theRenderContext.CSSetTexture(mLayer->GetTileTexture(int2(x, y)), 0);
			theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

			// pong
			theRenderContext.CSSetRWTexture(mLayer->GetTileRenderTarget(int2(x, y)), 0, 0);
			theRenderContext.CSSetTexture(mHistoryLayer->GetTileTexture(int2(x, y)), 0);
			theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

			theRenderContext.CSSetConstantBuffer(NULL, 0);
			theRenderContext.CSSetRWTexture(NULL, 0, 0);
			theRenderContext.CSSetTexture(NULL, 0);
			theRenderContext.CSSetShader(NULL);
		}
	}
}