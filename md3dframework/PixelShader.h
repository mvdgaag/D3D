#pragma once
#include "BaseResource.h"
#include <d3d11_1.h>


class PixelShader : public BaseResource
{
public:
	PixelShader(std::string inName) : BaseResource(inName) {}
	~PixelShader() { CleanUp(); }

	void InitFromFile(std::string inFileName);
	ID3D11PixelShader* GetHandle() { return mHandle; }

	void CleanUp();
	ResourceType GetResourceType() const { return ResourceType::PIXEL_SHADER; };

private:
	ID3D11PixelShader* mHandle = nullptr;
};

