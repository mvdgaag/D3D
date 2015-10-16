#pragma once
#include "BaseResource.h"

struct ID3D11Texture2D;
struct ID3D11DepthStencilView;

class DepthStencilTarget : public BaseResource
{
	friend class RenderContext;

public:
	DepthStencilTarget() : BaseResource() {};
	~DepthStencilTarget() { CleanUp(); }

	void Init(unsigned int inWidth, unsigned int inHeight);
	void CleanUp();
	
	ResourceType GetResourceType() const { return ResourceType::DEPTH_STENCIL_TARGET; }

private:
	ID3D11Texture2D* mDepthStencil = nullptr;
	ID3D11DepthStencilView*	mDepthStencilView = nullptr;
	unsigned int mWidth, mHeight;
};

