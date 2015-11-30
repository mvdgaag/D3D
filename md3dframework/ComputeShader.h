#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"

struct ID3D11ComputeShader;

REGISTERCLASS(ComputeShader);

class ComputeShader : public BaseResource
{
	friend class RenderContext;
	friend class ResourceFactory;

public:
	void InitFromFile(std::string inFileName);
	
	void CleanUp() override;
	ResourceType GetResourceType() const override { return ResourceType::COMPUTE_SHADER; };

protected:
	ID3D11ComputeShader* mHandle = nullptr;
	
private:
	ComputeShader() : BaseResource() {}
	~ComputeShader() { CleanUp(); }

	ComputeShader(ComputeShader const&) = delete;
	void operator=(ComputeShader const&) = delete;
};

