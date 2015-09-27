#pragma once
#include "BaseResource.h"
#include <d3d11_1.h>


//predeclarations
class Texture;


class RenderTarget : public BaseResource
{
public:
	RenderTarget(std::string inName) : BaseResource(inName) {};
	~RenderTarget() { CleanUp(); }

	void						Init(int inWidth, int inHeight, int inMipLevels, DXGI_FORMAT inFormat);
	ID3D11Texture2D*			GetTexture() { return mTexture; }
	ID3D11RenderTargetView*		GetRenderTargetView() { return mRenderTargetView; }
	ID3D11ShaderResourceView*	GetShaderResourceView() { return mShaderResourceView; }

	void						CleanUp();
	ResourceType				GetResourceType() const { return ResourceType::RENDER_TARGET; };

private:
	D3D11_TEXTURE2D_DESC mTextureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC mRenderTargetDesc;
	ID3D11Texture2D* mTexture = nullptr;
	ID3D11ShaderResourceView* mShaderResourceView = nullptr;
	ID3D11RenderTargetView* mRenderTargetView = nullptr;
};


