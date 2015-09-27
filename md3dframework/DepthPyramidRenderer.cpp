#include "DepthPyramidRenderer.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "LinearAlgebra.h"
#include <assert.h>


DepthPyramidRenderer::DepthPyramidRenderer() : mInitialized(false) 
{
	for (int i = 0; i < kNumMipLevels; i++)
		mRenderTargets[i] = nullptr;
}


void DepthPyramidRenderer::Render(ID3D11DeviceContext* inDeviceContext)
{
	assert(mInitialized);
}


void DepthPyramidRenderer::Init(RenderTarget* inSource)
{
	mInitialized = true;
}


void DepthPyramidRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
	if (mTargetTexture)
	{
		delete mTargetTexture;
		mTargetTexture = NULL;
	}
	for (int i = 0; i < kNumMipLevels; i++)
	{
		delete mRenderTargets[i];
		mRenderTargets[i] = NULL;
	}
	mSourceTexture = NULL;

	mInitialized = false;
}

