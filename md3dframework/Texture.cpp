#include "Texture.h"
#include "DDSTextureLoader.h"
#include "RenderContext.h"
#include <string>
#include <assert.h>
#include <d3d11.h>


int Texture::GetWidth()				
{
	return mDesc->Width; 
}


int Texture::GetHeight()
{
	return mDesc->Height; 
}


int Texture::GetMipLevels()
{
	return mDesc->MipLevels; 
}


void Texture::Init(int inWidth, int inHeight, int inMipLevels, unsigned int inFormat, unsigned int inBindFlags)
{
	CleanUp();

	mDesc = new D3D11_TEXTURE2D_DESC();
	ZeroMemory(mDesc, sizeof(D3D11_TEXTURE2D_DESC));
	mDesc->Width = inWidth;
	mDesc->Height = inHeight;
	mDesc->MipLevels = mDesc->ArraySize = inMipLevels;
	mDesc->Format = (DXGI_FORMAT)inFormat;
	mDesc->SampleDesc.Count = 1;
	mDesc->SampleDesc.Quality = 0;
	mDesc->Usage = D3D11_USAGE_DEFAULT;
	mDesc->BindFlags = inBindFlags;
	mDesc->CPUAccessFlags = 0;
	mDesc->MiscFlags = 0;

	D3DCall(theRenderContext.GetDevice()->CreateTexture2D(mDesc, NULL, &mTexture));
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	// HACK, needs to be different (not typeless) for depth-stencil
	shaderResourceViewDesc.Format = (DXGI_FORMAT)inFormat == DXGI_FORMAT_R24G8_TYPELESS ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : (DXGI_FORMAT)inFormat;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	D3DCall(theRenderContext.GetDevice()->CreateShaderResourceView(mTexture, &shaderResourceViewDesc, &mShaderResourceView));

	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);
}


void Texture::Init(ID3D11Texture2D* inTexture)
{
	CleanUp();

	assert(inTexture);
	
	mTexture = inTexture;
	mDesc = new D3D11_TEXTURE2D_DESC();
	mTexture->GetDesc(mDesc);
	
	D3DCall(theRenderContext.GetDevice()->CreateShaderResourceView(mTexture, NULL, &mShaderResourceView));
	
	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);
}


void Texture::InitFromFile(std::string inFileName)
{
	CleanUp();

	std::wstring filename = std::wstring(inFileName.begin(), inFileName.end());

	D3DCall(DirectX::CreateDDSTextureFromFile(theRenderContext.GetDevice(), filename.c_str(), nullptr, &mShaderResourceView));

	mShaderResourceView->GetResource(reinterpret_cast<ID3D11Resource**>(&mTexture));
	mDesc = new D3D11_TEXTURE2D_DESC();
	mTexture->GetDesc(mDesc);

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
	delete mDesc;
	mDesc = nullptr;
}
