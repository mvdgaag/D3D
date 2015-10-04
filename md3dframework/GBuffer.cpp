#include "GBuffer.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "main.h"
#include "Framework.h"
#include <assert.h>


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

	mRenderTargets[LINEAR_DEPTH] = new RenderTarget("GBuffer_Linear_Depth_Target");
	mRenderTargets[LINEAR_DEPTH]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R32_FLOAT);

	mRenderTargets[NORMAL] = new RenderTarget("GBuffer_Normal_Target");
	mRenderTargets[NORMAL]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R16G16_FLOAT);

	mRenderTargets[DIFFUSE] = new RenderTarget("GBuffer_Diffuse_Target");
	mRenderTargets[DIFFUSE]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mRenderTargets[MATERIAL] = new RenderTarget("GBuffer_Material_Target");
	mRenderTargets[MATERIAL]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mRenderTargets[MOTION_VECTORS] = new RenderTarget("GBuffer_Motion_Vector_Target");
	mRenderTargets[MOTION_VECTORS]->Init(mWidth, mHeight, 1, DXGI_FORMAT_R16G16_FLOAT);

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = mWidth;
	descDepth.Height = mHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	D3DCall(theFramework.GetDevice()->CreateTexture2D(&descDepth, nullptr, &mDepthStencil));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	D3DCall(theFramework.GetDevice()->CreateDepthStencilView(mDepthStencil, &descDSV, &mDepthStencilView));

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
	if (mDepthStencil != nullptr)
	{
		mDepthStencil->Release();
		mDepthStencil = nullptr;
	}
	if (mDepthStencilView != nullptr)
	{
		mDepthStencilView->Release();
		mDepthStencilView = nullptr;
	}
	mInitialized = false;
}


std::vector<ID3D11RenderTargetView*>GBuffer::GetRenderTargetViewArray()
{
	assert(mInitialized == true);
	std::vector<ID3D11RenderTargetView*> targets;
	for (int i = 0; i < NUM_RENDER_TARGETS; i++)
		targets.push_back(mRenderTargets[i]->GetRenderTargetView(0));
	return targets;
}