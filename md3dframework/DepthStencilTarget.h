#pragma once
#include "BaseResource.h"
#include "GAAGCommon.h"

struct ID3D11DepthStencilView;

PREDEFINE(Texture, pTexture);
PREDEFINE(DepthStencilTarget, pDepthStencilTarget);

class DepthStencilTarget : public BaseResource
{
	friend class RenderContext;

public:
	DepthStencilTarget() : BaseResource() {};
	~DepthStencilTarget() { CleanUp(); }

	void Init(int inWidth, int inHeight);
	void CleanUp() override;
	
	ResourceType GetResourceType() const { return ResourceType::DEPTH_STENCIL_TARGET; }

protected:
	pTexture				mTexture;
	ID3D11DepthStencilView*	mDepthStencilView = nullptr;

private:
	DepthStencilTarget(DepthStencilTarget const&) = delete;
	void operator=(DepthStencilTarget const&) = delete;
};

