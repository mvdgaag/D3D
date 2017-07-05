#include "SimpleHeightFlow.h"
#include "Terrain.h"
#include "Layer.h"
#include "ConstantBuffer.h"
#include "ComputeShader.h"



void SimpleHeightFlow::Init(pTerrain inTerrain)
{
	CleanUp();

	assert(inTerrain != nullptr);
	
	mTerrain = inTerrain;
	mResolution = inTerrain->GetTileResolution();
	mScale = inTerrain->GetTileScale();

	mFluidity = 10.0f;
	mFriction = 0.9f;
	mTalusSlope = 1.0f;

	mLayer = new Layer();
	mLayer->Init(inTerrain->GetNumTiles(), mResolution, Format::FORMAT_R16G16B16A16_FLOAT);
	mLayerIndex = inTerrain->SetLayer(mLayer);

	mHistoryLayer = new Layer();
	mHistoryLayer->Init(inTerrain->GetNumTiles(), mResolution, Format::FORMAT_R16G16B16A16_FLOAT);
	mHistoryLayerIndex = inTerrain->SetLayer(mHistoryLayer);

	mUpdateShader = theResourceFactory.LoadComputeShader("Shaders/UpdateSimpleHeightFlow.hlsl");
	mUpdateBorderShader = theResourceFactory.LoadComputeShader("Shaders/UpdateSimpleHeightFlowBorder.hlsl");
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
	if (mConstantBuffer != nullptr)
	{
		theResourceFactory.DestroyItem(mConstantBuffer);
		mConstantBuffer = nullptr;
	}

	if (mUpdateShader != nullptr)
	{
		theResourceFactory.DestroyItem(mUpdateShader);
		mUpdateShader = nullptr;
	}

	if (mUpdateBorderShader != nullptr)
	{
		theResourceFactory.DestroyItem(mUpdateBorderShader);
		mUpdateBorderShader = nullptr;
	}

	mInitialized = false;
}


void SimpleHeightFlow::Update(float inTimeStep)
{
	// DEVHACK
//	assert(0.5 * inTimeStep * mFluidity <= 1.0);

	if (!mInitialized)
		return;

	mConstantBufferData.params = float4(min(1.0, mFluidity * 0.5 * inTimeStep), mFriction, mTalusSlope, 0.0);
	mConstantBufferData.resolution = float4(float2(mLayer->GetTileResolution()), float2(1.0, 1.0) / float2(mLayer->GetTileResolution()));
	mConstantBufferData.scale = float4(mScale.x / mResolution.x, mScale.y / mResolution.y, mScale.z, 0.0);

	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);

	for (int y = 0; y < mLayer->GetNumTiles().y; y++)
	{
		for (int x = 0; x < mLayer->GetNumTiles().x; x++)
		{
			int2 tile_index = int2(x, y);
			int2 num_threads = (mResolution + 7) / 8;

			// ping center
			theRenderContext.CSSetShader(mUpdateShader);
			theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);
			theRenderContext.CSSetRWTexture(mHistoryLayer->GetTileRenderTarget(tile_index), 0, 0);
			theRenderContext.CSSetTexture(mLayer->GetTileTexture(tile_index), 0);
			theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

			// ping borders
			theRenderContext.CSSetShader(mUpdateBorderShader);
			theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);
			theRenderContext.CSSetRWTexture(mHistoryLayer->GetTileRenderTarget(tile_index), 0, 0);
			theRenderContext.CSSetTexture(mLayer->GetTileTexture(tile_index), 0);
			theRenderContext.CSSetTexture(mLayer->GetTileTexture(tile_index + int2(0, 1)), 1);
			theRenderContext.CSSetTexture(mLayer->GetTileTexture(tile_index + int2(1, 0)), 2);
			theRenderContext.CSSetTexture(mLayer->GetTileTexture(tile_index - int2(0, 1)), 3);
			theRenderContext.CSSetTexture(mLayer->GetTileTexture(tile_index - int2(1, 0)), 4);
			theRenderContext.Dispatch(num_threads.x, 4, 1); // assumes square texture
			
			// pong center
			theRenderContext.CSSetShader(mUpdateShader);
			theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);
			theRenderContext.CSSetRWTexture(mLayer->GetTileRenderTarget(tile_index), 0, 0);
			theRenderContext.CSSetTexture(mHistoryLayer->GetTileTexture(tile_index), 0);
			theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

			// pong borders
			theRenderContext.CSSetShader(mUpdateBorderShader);
			theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);
			theRenderContext.CSSetRWTexture(mLayer->GetTileRenderTarget(tile_index), 0, 0);
			theRenderContext.CSSetTexture(mHistoryLayer->GetTileTexture(tile_index), 0);
			theRenderContext.CSSetTexture(mHistoryLayer->GetTileTexture(tile_index + int2(0, 1)), 1);
			theRenderContext.CSSetTexture(mHistoryLayer->GetTileTexture(tile_index + int2(1, 0)), 2);
			theRenderContext.CSSetTexture(mHistoryLayer->GetTileTexture(tile_index - int2(0, 1)), 3);
			theRenderContext.CSSetTexture(mHistoryLayer->GetTileTexture(tile_index - int2(1, 0)), 4);
			theRenderContext.Dispatch(num_threads.x, 4, 1); // assumes square texture

			// reset state
			theRenderContext.CSSetConstantBuffer(NULL, 0);
			theRenderContext.CSSetRWTexture(NULL, 0, 0);
			theRenderContext.CSSetTexture(NULL, 0);
			theRenderContext.CSSetTexture(NULL, 1);
			theRenderContext.CSSetTexture(NULL, 2);
			theRenderContext.CSSetTexture(NULL, 3);
			theRenderContext.CSSetTexture(NULL, 4);
			theRenderContext.CSSetShader(NULL);

			mLayer->SetDirty(int2(x, y));
		}
	}
}