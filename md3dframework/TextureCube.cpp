/*
#include "TextureCube.h"
#include "DDSTextureLoader.h"
#include "RenderContext.h"
#include <string>
#include <assert.h>
#include <d3d11.h>
#include "FileUtil.h"


void TextureCube::Init(int inWidth, int inHeight, int inMipLevels, Format inFormat, unsigned int inUsage, BindFlag inBindFlag, CPUAccessFlag inCPUAccessFlags)
{
	CleanUp();

	mDesc = new D3D11_TEXTURE3D_DESC();
	ZeroMemory(mDesc, sizeof(D3D11_TEXTURE3D_DESC));
	mWidth = mDesc->Width = inWidth;
	mHeight = mDesc->Height = inHeight;
	mMipLevels = mDesc->MipLevels = mDesc->ArraySize = inMipLevels;

	// HACK, needs to be different (not typeless) for depth-stencil
	DXGI_FORMAT format = ((DXGI_FORMAT)inFormat == DXGI_FORMAT_R24G8_TYPELESS ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : (DXGI_FORMAT)inFormat);

	mFormat = Format(format);
	mDesc->Format = (DXGI_FORMAT)inFormat;

	mDesc->SampleDesc.Count = 1;

	mDesc->SampleDesc.Quality = 0;

	mDesc->Usage = (D3D11_USAGE)inUsage;

	mBindFlags = inBindFlags;
	mDesc->BindFlags = (D3D11_BIND_FLAG)inBindFlags;

	mCPUAccessFlags = inCPUAccessFlags;
	mDesc->CPUAccessFlags = (D3D11_CPU_ACCESS_FLAG)inCPUAccessFlags;
	mDesc->MiscFlags = 0;

	D3DCall(theRenderContext.GetDevice()->CreateTexture3D(mDesc, NULL, &mTexture));
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;
	D3DCall(theRenderContext.GetDevice()->CreateShaderResourceView(mTexture, &shaderResourceViewDesc, &mShaderResourceView));

	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);

	SetMipShaderResourceViews();
}


void TextureCube::Init(ID3D11Texture3D* inTexture)
{
	CleanUp();

	assert(inTexture);

	mTexture = inTexture;
	mDesc = new D3D11_TEXTURE3D_DESC();
	mTexture->GetDesc(mDesc);

	D3DCall(theRenderContext.GetDevice()->CreateShaderResourceView(mTexture, NULL, &mShaderResourceView));

	mWidth = mDesc->Width;
	mHeight = mDesc->Height;
	mMipLevels = mDesc->MipLevels;
	mFormat = (Format)mDesc->Format;
	mBindFlags = (BindFlag)mDesc->BindFlags;
	mCPUAccessFlags = (CPUAccessFlag)mDesc->CPUAccessFlags;

	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);

	SetMipShaderResourceViews();
}


void TextureCube::InitFromFile(std::string inFileName)
{
	assert(FileUtil::FileExists(inFileName.c_str()));

	CleanUp();

	std::wstring filename = std::wstring(inFileName.begin(), inFileName.end());

	D3DCall(DirectX::CreateDDSTextureFromFile(theRenderContext.GetDevice(), filename.c_str(), nullptr, &mShaderResourceView));

	mShaderResourceView->GetResource(reinterpret_cast<ID3D11Resource**>(&mTexture));
	mDesc = new D3D11_TEXTURE2D_DESC();
	mTexture->GetDesc(mDesc);

	mWidth = mDesc->Width;
	mHeight = mDesc->Height;
	mMipLevels = mDesc->MipLevels;
	mFormat = (Format)mDesc->Format;
	mBindFlags = (BindFlag)mDesc->BindFlags;
	mCPUAccessFlags = (CPUAccessFlag)mDesc->CPUAccessFlags;

	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);

	SetMipShaderResourceViews();
}

void TextureCube::CleanUp()
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
	if (mDesc)
	{
		delete mDesc;
		mDesc = nullptr;
	}
	if (mMipShaderResourceViews)
	{
		for (int i = 0; i < mMipLevels; i++)
			mMipShaderResourceViews[i]->Release();
		delete[] mMipShaderResourceViews;
		mMipShaderResourceViews = nullptr;
	}
}


void TextureCube::SetMipShaderResourceViews()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = (DXGI_FORMAT)mFormat;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	mMipShaderResourceViews = new ID3D11ShaderResourceView*[mMipLevels];
	for (int i = 0; i < mMipLevels; i++)
	{
		shaderResourceViewDesc.Texture2D.MostDetailedMip = i;
		D3DCall(theRenderContext.GetDevice()->CreateShaderResourceView(mTexture, &shaderResourceViewDesc, &mMipShaderResourceViews[i]));
		assert(mMipShaderResourceViews[i] != nullptr);
	}
}
*/