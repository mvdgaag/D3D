#pragma once
#include "BaseResource.h"
#include <d3d11_1.h>


class Texture : public BaseResource
{
public:
	Texture() : BaseResource() {};
	~Texture() { CleanUp(); }

	void Init(int inWidth, int inHeight, int inMipLevels, 
		DXGI_FORMAT inFormat, unsigned int inBindFlags = D3D11_BIND_SHADER_RESOURCE);
	void Init(ID3D11Texture2D* inTexture);
	void InitFromFile(std::string inFileName);

	ID3D11Texture2D*			GetTexture()			{ return mTexture; }
	ID3D11ShaderResourceView*	GetShaderResourceView()	{ return mShaderResourceView; }
	int							GetWidth()				{ return mDesc.Width; }
	int							GetHeight()				{ return mDesc.Height; }
	int							GetMipLevels()			{ return mDesc.MipLevels; }
	DXGI_FORMAT					GetFormat()				{ return mDesc.Format; }
	unsigned int				GetBindFlags()			{ return mDesc.BindFlags; }

	void CleanUp();
	ResourceType GetResourceType() const { return ResourceType::TEXTURE; };
private:
	ID3D11Texture2D*			mTexture = nullptr;
	ID3D11ShaderResourceView*	mShaderResourceView = nullptr;
	D3D11_TEXTURE2D_DESC		mDesc;
};

