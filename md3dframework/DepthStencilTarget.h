#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"

struct ID3D11DepthStencilView;

REGISTERCLASS(Texture);
REGISTERCLASS(DepthStencilTarget);

class DepthStencilTarget : public BaseResource
{
	friend class ResourceFactory;
	friend class RenderContext;

public:
	void Init(int inWidth, int inHeight);
	void CleanUp() override;
	
	ResourceType GetResourceType() const { return ResourceType::DEPTH_STENCIL_TARGET; }

protected:
	pTexture				mTexture;
	ID3D11DepthStencilView*	mDepthStencilView = nullptr;

private:
	DepthStencilTarget() : BaseResource() {};
	~DepthStencilTarget() { CleanUp(); }

	DepthStencilTarget(DepthStencilTarget const&) = delete;
	void operator=(DepthStencilTarget const&) = delete;
};

