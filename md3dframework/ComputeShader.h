#pragma once
#include "BaseResource.h"

struct ID3D11ComputeShader;

class ComputeShader : public BaseResource
{
	friend class RenderContext;

public:
	ComputeShader() : BaseResource() {}
	~ComputeShader() { CleanUp(); }

	void InitFromFile(std::string inFileName);
	
	void CleanUp() override;
	ResourceType GetResourceType() const override { return ResourceType::COMPUTE_SHADER; };

protected:
	ID3D11ComputeShader* mHandle = nullptr;
};

