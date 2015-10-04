#include "Texture.h"
#include "DDSTextureLoader.h"
#include "main.h"
#include "Framework.h"
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

	D3DCall(theFramework.GetDevice()->CreateTexture2D(&mDesc, NULL, &mTexture));
	D3DCall(theFramework.GetDevice()->CreateShaderResourceView(mTexture, NULL, &mShaderResourceView));

	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);
}


void Texture::InitFromFile(std::string inFileName)
{
	std::wstring filename = std::wstring(inFileName.begin(), inFileName.end());

	// TODO: set descriptor for this class (mDesc)
	D3DCall(DirectX::CreateDDSTextureFromFile(theFramework.GetDevice(), filename.c_str(), nullptr, &mShaderResourceView));

	// TODO fetch texture as well
//	assert(mTexture != nullptr);
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