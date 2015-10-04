#pragma once
#include "BaseResource.h"
#include <d3d11_1.h>
#include <assert.h>


//predeclarations
class Texture;


class RenderTarget : public BaseResource
{
public:
	RenderTarget(std::string inName) : BaseResource(inName) {};
	~RenderTarget() { CleanUp(); }

	void						Init(int inWidth, int inHeight, int inMipLevels, DXGI_FORMAT inFormat);
	ID3D11RenderTargetView*		GetRenderTargetView(int inMipLevel = 0);
	ID3D11UnorderedAccessView*	GetUnorderedAccessView(int inMipLevel = 0);
	Texture*					GetTexture() { return mTexture; }
	void						CleanUp();
	ResourceType				GetResourceType() const { return ResourceType::RENDER_TARGET; };

private:
	Texture* mTexture = nullptr;

	ID3D11RenderTargetView** mRenderTargetViews = nullptr;
	ID3D11UnorderedAccessView** mUnorderedAccessViews = nullptr;
};


