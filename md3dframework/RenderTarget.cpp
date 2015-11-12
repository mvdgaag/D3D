#include "RenderTarget.h"
#include "RenderContext.h"
#include "Texture.h"
#include <assert.h>
#include <d3d11_1.h>


void RenderTarget::Init(int inWidth, int inHeight, int inMipLevels, unsigned int inFormat)
{
	CleanUp();

	BindFlag bind_flags = BIND_RENDER_TARGET | BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
	pTexture texture = MAKE_NEW(Texture);
	texture->Init(inWidth, inHeight, inMipLevels, inFormat, 0, bind_flags);

	Init(texture);
}


void RenderTarget::Init(pTexture inTexture)
{
	CleanUp();

	// TODO: replace with smart pointers, does this target now own the texture for destruction?
	mTexture = inTexture;
	unsigned int required_flags = D3D11_BIND_RENDER_TARGET;
	assert((mTexture->mDesc->BindFlags & required_flags) == required_flags);

	mRenderTargetViews = new ID3D11RenderTargetView*[inTexture->GetMipLevels()];
	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
	ZeroMemory(&rtv_desc, sizeof(rtv_desc));
	rtv_desc.Format = mTexture->mDesc->Format;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	mUnorderedAccessViews = new ID3D11UnorderedAccessView*[inTexture->GetMipLevels()];
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
	ZeroMemory(&uav_desc, sizeof(uav_desc));
	uav_desc.Format = mTexture->mDesc->Format;
	uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < mTexture->GetMipLevels(); i++)
	{
		rtv_desc.Texture2D.MipSlice = i;
		D3DCall(theRenderContext.GetDevice()->CreateRenderTargetView(mTexture->mTexture, &rtv_desc, &mRenderTargetViews[i]));
		assert(mRenderTargetViews[i] != nullptr);

		uav_desc.Texture2D.MipSlice = i;
		D3DCall(theRenderContext.GetDevice()->CreateUnorderedAccessView(mTexture->mTexture, &uav_desc, &mUnorderedAccessViews[i]));
		assert(mUnorderedAccessViews[i] != nullptr);
	}
}


void RenderTarget::CleanUp()
{
	if (mUnorderedAccessViews != nullptr)
	{
		for (int i = 0; i < mTexture->GetMipLevels(); i++)
			mUnorderedAccessViews[i]->Release();
		delete[] mUnorderedAccessViews;
		mUnorderedAccessViews = nullptr;
	}
	if (mRenderTargetViews != nullptr)
	{
		for (int i = 0; i < mTexture->GetMipLevels(); i++)
			mRenderTargetViews[i]->Release();
		delete[] mRenderTargetViews;
		mRenderTargetViews = nullptr;
	}

	mTexture = nullptr;
}
