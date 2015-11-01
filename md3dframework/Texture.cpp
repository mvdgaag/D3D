#include "Texture.h"
#include "DDSTextureLoader.h"
#include "RenderContext.h"
#include <string>
#include <assert.h>
#include <d3d11.h>
#include "FileUtil.h"


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


void Texture::Init(int inWidth, int inHeight, int inMipLevels, unsigned int inFormat, 
	unsigned int inUsage, unsigned int inBindFlags, unsigned int inCPUAccessFlags)
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
	mDesc->Usage = (D3D11_USAGE)inUsage;
	mDesc->BindFlags = inBindFlags;
	mDesc->CPUAccessFlags = (D3D11_CPU_ACCESS_FLAG)inCPUAccessFlags;
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
	assert(FileUtil::FileExists(inFileName.c_str()));

	CleanUp();

	std::wstring filename = std::wstring(inFileName.begin(), inFileName.end());

	D3DCall(DirectX::CreateDDSTextureFromFile(theRenderContext.GetDevice(), filename.c_str(), nullptr, &mShaderResourceView));

	mShaderResourceView->GetResource(reinterpret_cast<ID3D11Resource**>(&mTexture));
	mDesc = new D3D11_TEXTURE2D_DESC();
	mTexture->GetDesc(mDesc);

	assert(mTexture != nullptr);
	assert(mShaderResourceView != nullptr);
}


float4 Texture::GetPixel(int2 inPixelCoordinate)
{
	D3D11_TEXTURE2D_DESC* tempDesc = new D3D11_TEXTURE2D_DESC();
	ZeroMemory(tempDesc, sizeof(tempDesc));
	tempDesc->Width = 1;
	tempDesc->Height = 1;
	tempDesc->MipLevels = tempDesc->ArraySize = 1;
	tempDesc->Format = mDesc->Format;
	tempDesc->SampleDesc.Count = 1;
	tempDesc->SampleDesc.Quality = 0;
	tempDesc->Usage = D3D11_USAGE_STAGING;
	tempDesc->BindFlags = 0;
	tempDesc->CPUAccessFlags = D3D10_CPU_ACCESS_READ;
	tempDesc->MiscFlags = 0;

	ID3D11Texture2D* tempTexture;
	D3DCall(theRenderContext.GetDevice()->CreateTexture2D(tempDesc, NULL, &tempTexture));
	
	Texture tmp;
	tmp.mDesc = tempDesc;
	tmp.mTexture = tempTexture;

	theRenderContext.CopySubResourceRegion(tmp, *this, rect(inPixelCoordinate, inPixelCoordinate + 1), int2(0));
	
	void* dataPtr;
	theRenderContext.Map(tmp, &dataPtr);
	
	float* floatPtr = (float*)dataPtr;
	unsigned char* bytePtr = (unsigned char*)dataPtr;

	float4 result = float4(0, 0, 0, 0);
	switch (mDesc->Format)
	{
	case DXGI_FORMAT_R32_FLOAT:
		result.x = floatPtr[0];
		break;
	case DXGI_FORMAT_R8_UNORM:
		result.x = (float)bytePtr[0] / 256.0;
		break;

	case DXGI_FORMAT_R32G32_FLOAT:
		result.x = floatPtr[0];
		result.y = floatPtr[1];
		break;
	case DXGI_FORMAT_R8G8_UNORM:
		result.x = (float)bytePtr[0] / 256.0;
		result.y = (float)bytePtr[1] / 256.0;
		break;

	case DXGI_FORMAT_R32G32B32_FLOAT:
		result.x = floatPtr[0];
		result.y = floatPtr[1];
		result.z = floatPtr[2];
		break;

	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		result.x = floatPtr[0];
		result.y = floatPtr[1];
		result.z = floatPtr[2];
		result.w = floatPtr[3];
		break;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		result.x = (float)bytePtr[0] / 256.0;
		result.y = (float)bytePtr[1] / 256.0;
		result.z = (float)bytePtr[2] / 256.0;
		result.w = (float)bytePtr[3] / 256.0;
		break;
	}
	
	theRenderContext.UnMap(tmp);

	return result;
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
	if (mDesc)
	{
		delete mDesc;
		mDesc = nullptr;
	}
}
