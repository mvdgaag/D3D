#include "Texture.h"
#include "DDSTextureLoader.h"
#include "main.h"
#include "FrameWork.h"
#include <string>
#include <assert.h>


void Texture::Init(int inWidth, int inHeight, int inMipLevels, DXGI_FORMAT inFormat)
{
	CleanUp();

	ZeroMemory(&mDesc, sizeof(mDesc));
	mDesc.Width = inWidth;
	mDesc.Height = inHeight;
	mDesc.MipLevels = mDesc.ArraySize = inMipLevels;
	mDesc.Format = inFormat;
	mDesc.SampleDesc.Count = 1;
	mDesc.Usage = D3D11_USAGE_DYNAMIC;
	mDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	mDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mDesc.MiscFlags = 0;

	D3DCall(theFrameWork.GetDevice()->CreateTexture2D(&mDesc, NULL, &mTexture));
	D3DCall(theFrameWork.GetDevice()->CreateShaderResourceView(mTexture, NULL, &mShaderResourceView));

	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);
}


void Texture::InitFromFile(std::string inFileName)
{
	std::wstring filename = std::wstring(inFileName.begin(), inFileName.end());

	// TODO: set descriptor for this class (mDesc)
	D3DCall(DirectX::CreateDDSTextureFromFile(theFrameWork.GetDevice(), filename.c_str(), nullptr, &mShaderResourceView));

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