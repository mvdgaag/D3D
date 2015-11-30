#pragma once
#include "BaseResource.h"
#include <hash_map>
#include "GaagCommon.h"

struct ID3D11PixelShader;

REGISTERCLASS(PixelShader);

class PixelShader : public BaseResource
{
	friend class ResourceFactory;
	friend class RenderContext;

public:
	void InitFromFile(std::string inFileName);
	void CleanUp() override;
	
	ResourceType GetResourceType() const override { return ResourceType::PIXEL_SHADER; }

protected:
	ID3D11PixelShader* mHandle = nullptr;
	std::hash_map<std::string, unsigned int> mTextures;
	std::hash_map<std::string, unsigned int> mSamplers;
	std::hash_map<std::string, unsigned int> mConstantBuffers;

private:
	PixelShader() : BaseResource() {}
	~PixelShader() { CleanUp(); }

	PixelShader(PixelShader const&) = delete;
	void operator=(PixelShader const&) = delete;
};
