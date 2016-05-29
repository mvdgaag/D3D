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

	// initializes the rendertarget with a new texture that it will own and destroy on cleanup
	void						Init(int inWidth, int inHeight, int inMipLevels, Format inFormat);

	// initializes the rendertarget with a new texture. Nota that it will NOT own the texture!
	void						Init(pTexture inTexture);

	// cleans up the data. Note that this doesn't necessarily own the texture!
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

	bool mOwnsTexture = false;
	bool mInitialized = false;

	RenderTarget(RenderTarget const&) = delete;
	void operator=(RenderTarget const&) = delete;
};
