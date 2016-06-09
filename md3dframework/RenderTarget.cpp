#include "RenderTarget.h"
#include "RenderContext.h"
#include "Texture.h"
#include <assert.h>
#include <d3d11_1.h>


void RenderTarget::Init(int inWidth, int inHeight, int inMipLevels, Format inFormat)
{
	assert(!mInitialized);

	BindFlag bind_flags = BIND_RENDER_TARGET | BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
	pTexture texture = MAKE_NEW(Texture);
	texture->Init(inWidth, inHeight, inMipLevels, inFormat, 0, bind_flags);
	mOwnsTexture = true;

	Init(texture);
}


void RenderTarget::Init(pTexture inTexture)
{
	assert(!mInitialized);

	mTexture = inTexture;
	unsigned int required_flags = D3D11_BIND_RENDER_TARGET;
	assert((mTexture->mDesc->BindFlags & required_flags) == required_flags);

	if (mTexture->IsCubemap())
	{
		mRenderTargetViews = new ID3D11RenderTargetView*[inTexture->GetMipLevels() * 6];
		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
		ZeroMemory(&rtv_desc, sizeof(rtv_desc));
		rtv_desc.Format = mTexture->mDesc->Format;
		rtv_desc.Texture2DArray.ArraySize = 6;
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;

		mUnorderedAccessViews = new ID3D11UnorderedAccessView*[inTexture->GetMipLevels() * 6];
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
		ZeroMemory(&uav_desc, sizeof(uav_desc));
		uav_desc.Format = mTexture->mDesc->Format;
		uav_desc.Texture2DArray.ArraySize = 6;
		rtv_desc.Texture2DArray.FirstArraySlice = 0;
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;

		for (int i = 0; i < mTexture->GetMipLevels(); i++)
		{
			rtv_desc.Texture2DArray.MipSlice = i;
			D3DCall(theRenderContext.GetDevice()->CreateRenderTargetView(mTexture->mTexture, &rtv_desc, &mRenderTargetViews[i]));
			assert(mRenderTargetViews[i] != nullptr);

			uav_desc.Texture2DArray.MipSlice = i;
			D3DCall(theRenderContext.GetDevice()->CreateUnorderedAccessView(mTexture->mTexture, &uav_desc, &mUnorderedAccessViews[i]));
			assert(mUnorderedAccessViews[i] != nullptr);
		}
	}
	else
	{
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

	mInitialized = true;
}


void RenderTarget::CleanUp()
{
	int array_size = mTexture->GetMipLevels();

	if (mUnorderedAccessViews != nullptr)
	{
		for (int i = 0; i <array_size; i++)
			mUnorderedAccessViews[i]->Release();
		delete[] mUnorderedAccessViews;
		mUnorderedAccessViews = nullptr;
	}

	if (mRenderTargetViews != nullptr)
	{
		for (int i = 0; i < array_size; i++)
			mRenderTargetViews[i]->Release();
		delete[] mRenderTargetViews;
		mRenderTargetViews = nullptr;
	}

	if (mOwnsTexture)
		mTexture->CleanUp();

	mTexture = nullptr;
}
