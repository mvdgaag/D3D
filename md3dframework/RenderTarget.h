#pragma once
#include "BaseResource.h"
#include <assert.h>
#include <d3d11_1.h>


class Texture;


class RenderTarget : public BaseResource
{
public:
	RenderTarget() : BaseResource() {};
	~RenderTarget() { CleanUp(); }

	void						Init(int inWidth, int inHeight, int inMipLevels, DXGI_FORMAT inFormat);
	void						Init(Texture* inTexture);
	ID3D11RenderTargetView*		GetRenderTargetView(int inMipLevel = 0);
	ID3D11UnorderedAccessView*	GetUnorderedAccessView(int inMipLevel = 0);
	Texture*					GetTexture() { return mTexture; }
	void						CleanUp();
	ResourceType				GetResourceType() const { return ResourceType::RENDER_TARGET; }

private:
	Texture* mTexture = nullptr;

	ID3D11RenderTargetView** mRenderTargetViews = nullptr;
	ID3D11UnorderedAccessView** mUnorderedAccessViews = nullptr;
};


