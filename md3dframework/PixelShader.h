#pragma once
#include "BaseResource.h"
#include <hash_map>

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
	std::hash_map<std::string, unsigned int> mTextures;
	std::hash_map<std::string, unsigned int> mSamplers;
	std::hash_map<std::string, unsigned int> mConstantBuffers;
};

