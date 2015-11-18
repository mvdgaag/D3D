#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"
#include "Texture.h"

struct ID3D11RenderTargetView;
struct ID3D11UnorderedAccessView;

PREDEFINE(RenderTarget, pRenderTarget);


class RenderTarget : public BaseResource
{
	friend class RenderContext;

public:
	RenderTarget() : BaseResource() {};
	~RenderTarget() { CleanUp(); }

	pTexture					GetTexture() { return mTexture; }
	void						Init(int inWidth, int inHeight, int inMipLevels, Format inFormat);
	void						Init(pTexture inTexture);
	void						CleanUp() override;
	ResourceType				GetResourceType() const override { return ResourceType::RENDER_TARGET; }

protected:
	pTexture mTexture;
	ID3D11RenderTargetView** mRenderTargetViews = nullptr;
	ID3D11UnorderedAccessView** mUnorderedAccessViews = nullptr;

private:
	RenderTarget(RenderTarget const&) = delete;
	void operator=(RenderTarget const&) = delete;
};
