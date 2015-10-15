#pragma once
#include "BaseResource.h"
#include <d3d11_1.h>


class DepthStencilTarget : public BaseResource
{
public:
	DepthStencilTarget() : BaseResource() {};
	~DepthStencilTarget() { CleanUp(); }

	void Init(unsigned int inWidth, unsigned int inHeight);
	void CleanUp();
	ID3D11DepthStencilView* GetDepthStencilView() { return mDepthStencilView; }
	ResourceType GetResourceType() const { return ResourceType::DEPTH_STENCIL_TARGET; }

private:
	ID3D11Texture2D* mDepthStencil = nullptr;
	ID3D11DepthStencilView*	mDepthStencilView = nullptr;
	unsigned int mWidth, mHeight;
};

