#pragma once
#include "BaseResource.h"
#include "GAAGCommon.h"

struct ID3D11RenderTargetView;
struct ID3D11UnorderedAccessView;

PREDEFINE(Texture, pTexture);
PREDEFINE(RenderTarget, pRenderTarget);

class RenderTarget : public BaseResource
{
	friend class RenderContext;

public:
	RenderTarget() : BaseResource() {};
	~RenderTarget() { CleanUp(); }

	Texture*					GetTexture() { return mTexture; }

	void						Init(int inWidth, int inHeight, int inMipLevels, unsigned int inFormat);
	void						Init(Texture* inTexture);
	void						CleanUp() override;

	ResourceType				GetResourceType() const override { return ResourceType::RENDER_TARGET; }

protected:
	Texture* mTexture = nullptr;
	ID3D11RenderTargetView** mRenderTargetViews = nullptr;
	ID3D11UnorderedAccessView** mUnorderedAccessViews = nullptr;
};
