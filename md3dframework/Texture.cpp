#include "Texture.h"
#include "DDSTextureLoader.h"
#include "RenderContext.h"
#include <string>
#include <assert.h>


void Texture::Init(int inWidth, int inHeight, int inMipLevels, DXGI_FORMAT inFormat, unsigned int inBindFlags)
{
	CleanUp();

	ZeroMemory(&mDesc, sizeof(mDesc));
	mDesc.Width = inWidth;
	mDesc.Height = inHeight;
	mDesc.MipLevels = mDesc.ArraySize = inMipLevels;
	mDesc.Format = inFormat;
	mDesc.SampleDesc.Count = 1;
	mDesc.Usage = D3D11_USAGE_DEFAULT;
	mDesc.BindFlags = inBindFlags;
	mDesc.CPUAccessFlags = 0;
	mDesc.MiscFlags = 0;

	D3DCall(theRenderContext.GetDevice()->CreateTexture2D(&mDesc, NULL, &mTexture));
	D3DCall(theRenderContext.GetDevice()->CreateShaderResourceView(mTexture, NULL, &mShaderResourceView));

	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);
}


void Texture::Init(ID3D11Texture2D* inTexture)
{
	assert(inTexture);
	
	mTexture = inTexture;
	mTexture->GetDesc(&mDesc);
	
	D3DCall(theRenderContext.GetDevice()->CreateShaderResourceView(mTexture, NULL, &mShaderResourceView));
	
	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);
}


void Texture::InitFromFile(std::string inFileName)
{
	std::wstring filename = std::wstring(inFileName.begin(), inFileName.end());

	// TODO: set descriptor for this class (mDesc)
	D3DCall(DirectX::CreateDDSTextureFromFile(theRenderContext.GetDevice(), filename.c_str(), nullptr, &mShaderResourceView));

	mShaderResourceView->GetResource(reinterpret_cast<ID3D11Resource**>(&mTexture));
	mTexture->GetDesc(&mDesc);

	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);
}


void Texture::CleanUp()
{
	if (mTexture != nullptr)
	{
		mTexture->Release();
		mTexture = nullptr;
	}
	if (mShaderResourceView != nullptr)
	{
		mShaderResourceView->Release();
		mShaderResourceView = nullptr;
	}
}