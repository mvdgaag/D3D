#pragma once
#include "BaseResource.h"
#include <assert.h>

struct ID3D11RenderTargetView;
struct ID3D11UnorderedAccessView;
class Texture;

class RenderTarget : public BaseResource
{
	friend class RenderContext;

public:
	RenderTarget() : BaseResource() {};
	~RenderTarget() { CleanUp(); }

	Texture*					GetTexture() { return mTexture; }

	void						Init(int inWidth, int inHeight, int inMipLevels, unsigned int inFormat);
	void						Init(Texture* inTexture);
	void						CleanUp();

	ResourceType				GetResourceType() const { return ResourceType::RENDER_TARGET; }

private:
	Texture* mTexture = nullptr;
	ID3D11RenderTargetView** mRenderTargetViews = nullptr;
	ID3D11UnorderedAccessView** mUnorderedAccessViews = nullptr;
};


