#include "DepthStencilTarget.h"
#include "RenderContext.h"
#include "Texture.h"
#include <d3d11_1.h>


void DepthStencilTarget::Init(int inWidth, int inHeight)
{
	CleanUp();

	mTexture = std::make_shared<Texture>();
	mTexture->Init(inWidth, inHeight, 1, DXGI_FORMAT_R24G8_TYPELESS, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	D3DCall(theRenderContext.GetDevice()->CreateDepthStencilView(mTexture->mTexture, &descDSV, &mDepthStencilView));
}


void DepthStencilTarget::CleanUp()
{
	mTexture = nullptr;
	if (mDepthStencilView != nullptr)
	{
		mDepthStencilView->Release();
		mDepthStencilView = nullptr;
	}
}
