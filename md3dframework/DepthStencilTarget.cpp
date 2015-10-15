#include "DepthStencilTarget.h"
#include "RenderContext.h"


void DepthStencilTarget::Init(unsigned int inWidth, unsigned int inHeight)
{
	CleanUp();
	mWidth = inWidth;
	mHeight = inHeight;

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
	D3DCall(theRenderContext.GetDevice()->CreateTexture2D(&descDepth, nullptr, &mDepthStencil));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	D3DCall(theRenderContext.GetDevice()->CreateDepthStencilView(mDepthStencil, &descDSV, &mDepthStencilView));
}


void DepthStencilTarget::CleanUp()
{
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
}