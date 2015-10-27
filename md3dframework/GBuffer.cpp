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

	mRenderTargets[LINEAR_DEPTH] = std::make_shared<RenderTarget>();
	mRenderTargets[LINEAR_DEPTH]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R32_FLOAT);

	mRenderTargets[NORMAL] = std::make_shared<RenderTarget>();
	mRenderTargets[NORMAL]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R16G16_FLOAT);

	mRenderTargets[DIFFUSE] = std::make_shared<RenderTarget>();
	mRenderTargets[DIFFUSE]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mRenderTargets[MATERIAL] = std::make_shared<RenderTarget>();
	mRenderTargets[MATERIAL]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mRenderTargets[MOTION_VECTORS] = std::make_shared<RenderTarget>();
	mRenderTargets[MOTION_VECTORS]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R16G16_FLOAT);

	mDepthStencilTarget = std::make_shared<DepthStencilTarget>();
	mDepthStencilTarget->Init(mWidth, mHeight);

	mInitialized = true;
}



void GBuffer::CleanUp()
{
	for (int i = 0; i < NUM_RENDER_TARGETS; i++)
		mRenderTargets[i] = nullptr;
	mDepthStencilTarget = nullptr;
	mInitialized = false;
}
