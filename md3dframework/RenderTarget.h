#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"
#include "Texture.h"

struct ID3D11RenderTargetView;
struct ID3D11UnorderedAccessView;
struct ID3D11ShaderResourceView;

REGISTERCLASS(RenderTarget);


class RenderTarget : public BaseResource
{
	friend class ResourceFactory;
	friend class RenderContext;

public:
	pTexture					GetTexture() { return mTexture; }
	void						Init(int inWidth, int inHeight, int inMipLevels, Format inFormat);
	void						Init(pTexture inTexture);
	void						CleanUp() override;
	
	int2						GetDimensions() { return mTexture->GetDimensions(); }
	ResourceType				GetResourceType() const override { return ResourceType::RENDER_TARGET; }

protected:
	pTexture mTexture;
	ID3D11RenderTargetView** mRenderTargetViews = nullptr;
	ID3D11UnorderedAccessView** mUnorderedAccessViews = nullptr;

private:
	RenderTarget() : BaseResource() {};
	~RenderTarget() { CleanUp(); }

	RenderTarget(RenderTarget const&) = delete;
	void operator=(RenderTarget const&) = delete;
};
