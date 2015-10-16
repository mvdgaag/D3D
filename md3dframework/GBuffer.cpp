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
	mRenderTargets = nullptr;
}


void GBuffer::Init(int inWidth, int inHeight)
{
	CleanUp();

	mRenderTargets = new RenderTarget*[NUM_RENDER_TARGETS];

	mWidth = inWidth;
	mHeight = inHeight;

	mRenderTargets[LINEAR_DEPTH] = new RenderTarget();
	mRenderTargets[LINEAR_DEPTH]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R32_FLOAT);

	mRenderTargets[NORMAL] = new RenderTarget();
	mRenderTargets[NORMAL]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R16G16_FLOAT);

	mRenderTargets[DIFFUSE] = new RenderTarget();
	mRenderTargets[DIFFUSE]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mRenderTargets[MATERIAL] = new RenderTarget();
	mRenderTargets[MATERIAL]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mRenderTargets[MOTION_VECTORS] = new RenderTarget();
	mRenderTargets[MOTION_VECTORS]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R16G16_FLOAT);

	mDepthStencilTarget = new DepthStencilTarget();
	mDepthStencilTarget->Init(mWidth, mHeight);

	mInitialized = true;
}


Texture* GBuffer::GetTexture(GBufferType inType)				
{ 
	return mRenderTargets[inType]->GetTexture(); 
}


void GBuffer::CleanUp()
{
	if (mRenderTargets != nullptr)
	{
		for (int i = 0; i < NUM_RENDER_TARGETS; i++)
		{
			if (mRenderTargets[i] != nullptr)
			{
				delete mRenderTargets[i];
				mRenderTargets[i] = nullptr;
			}
		}
		delete[] mRenderTargets;
		mRenderTargets = nullptr;
	}
	delete mDepthStencilTarget;
	mInitialized = false;
}
