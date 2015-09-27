#include "main.h"
#include "RenderTarget.h"
#include "main.h"
#include "FrameWork.h"
#include "Texture.h"
#include <assert.h>


void RenderTarget::Init(int inWidth, int inHeight, int inMipLevels, DXGI_FORMAT inFormat)
{
	CleanUp();

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&mTextureDesc, sizeof(mTextureDesc));
	mTextureDesc.Width = inWidth;
	mTextureDesc.Height = inHeight;
	mTextureDesc.MipLevels = mTextureDesc.ArraySize = inMipLevels;
	mTextureDesc.Format = inFormat;
	mTextureDesc.SampleDesc.Count = 1;
	mTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	mTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	mTextureDesc.CPUAccessFlags = 0;
	mTextureDesc.MiscFlags = 0;

	D3DCall(theFrameWork.GetDevice()->CreateTexture2D(&mTextureDesc, NULL, &mTexture));
	assert(mTexture != nullptr);

	D3DCall(theFrameWork.GetDevice()->CreateShaderResourceView(mTexture, NULL, &mShaderResourceView));
	assert(mShaderResourceView != nullptr);

	ZeroMemory(&mRenderTargetDesc, sizeof(mRenderTargetDesc));
	mRenderTargetDesc.Format = mTextureDesc.Format;
	mRenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	mRenderTargetDesc.Texture2D.MipSlice = 0;

	D3DCall(theFrameWork.GetDevice()->CreateRenderTargetView(mTexture, &mRenderTargetDesc, &mRenderTargetView));
	assert(mRenderTargetView != nullptr);
}


void RenderTarget::CleanUp()
{
	if (mTexture != nullptr)
	{
		mTexture->Release();
		mTexture = nullptr;
	}
	if (mRenderTargetView != nullptr)
	{
		mRenderTargetView->Release();
		mRenderTargetView = nullptr;
	}
	if (mShaderResourceView != nullptr)
	{
		mShaderResourceView->Release();
		mShaderResourceView = nullptr;
	}
}
