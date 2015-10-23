#pragma once
#include "BaseResource.h"

struct ID3D11PixelShader;

class PixelShader : public BaseResource
{
	friend class RenderContext;

public:
	PixelShader() : BaseResource() {}
	~PixelShader() { CleanUp(); }

	void InitFromFile(std::string inFileName);
	void CleanUp() override;
	
	ResourceType GetResourceType() const override { return ResourceType::PIXEL_SHADER; };

protected:
	ID3D11PixelShader* mHandle = nullptr;
};

