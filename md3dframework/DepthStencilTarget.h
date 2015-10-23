#pragma once
#include "BaseResource.h"
#include "Texture.h"
struct ID3D11DepthStencilView;

class DepthStencilTarget : public Texture
{
	friend class RenderContext;

public:
	DepthStencilTarget() : Texture() {};
	~DepthStencilTarget() { CleanUp(); }

	// depricate functions, should throw exception or go for factory class model?
	void Init(int inWidth, int inHeight, int inMipLevels, unsigned int inFormat, unsigned int inBindFlags = 8) override
	{}
	void Init(ID3D11Texture2D* inTexture) override
	{}
	void InitFromFile(std::string inFileName) override
	{}
	
	void Init(int inWidth, int inHeight);
	void CleanUp() override;
	
	ResourceType GetResourceType() const { return ResourceType::DEPTH_STENCIL_TARGET; }

protected:
	ID3D11DepthStencilView*	mDepthStencilView = nullptr;
};

