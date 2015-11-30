#include "GBuffer.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "DepthStencilTarget.h"
#include "RenderContext.h"
#include <assert.h>
#include <d3d11_1.h>


GBuffer::GBuffer()
{
	mInitialized = false;
}


void GBuffer::Init(int inWidth, int inHeight)
{
	CleanUp();

	mWidth = inWidth;
	mHeight = inHeight;

	mRenderTargets[LINEAR_DEPTH] = theResourceFactory.MakeRenderTarget(int2(mWidth, mHeight), 1, FORMAT_R32_FLOAT);
	mClearColors[LINEAR_DEPTH] = float4(9999999, 0, 0, 0);

	mRenderTargets[NORMAL] = theResourceFactory.MakeRenderTarget(int2(mWidth, mHeight), 1, FORMAT_R16G16_FLOAT);
	mClearColors[NORMAL] = float4(0, 0, 1, 0);

	mRenderTargets[DIFFUSE] = theResourceFactory.MakeRenderTarget(int2(mWidth, mHeight), 1, FORMAT_R8G8B8A8_UNORM);
	mClearColors[DIFFUSE] = float4(0, 0, 0, 0);

	mRenderTargets[MATERIAL] = theResourceFactory.MakeRenderTarget(int2(mWidth, mHeight), 1, FORMAT_R8G8B8A8_UNORM);
	mClearColors[MATERIAL] = float4(0, 0, 0, 0);

	mRenderTargets[MOTION_VECTORS] = theResourceFactory.MakeRenderTarget(int2(mWidth, mHeight), 1, FORMAT_R16G16_FLOAT);
	mClearColors[MOTION_VECTORS] = float4(0, 0, 0, 0);

	mDepthStencilTarget = theResourceFactory.MakeDepthStencilTarget(int2(mWidth, mHeight));

	mInitialized = true;
}



void GBuffer::CleanUp()
{
	for (int i = 0; i < NUM_RENDER_TARGETS; i++)
		mRenderTargets[i] = nullptr;
	mDepthStencilTarget = nullptr;
	mInitialized = false;
}
