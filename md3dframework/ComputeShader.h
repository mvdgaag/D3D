#pragma once
#include "BaseResource.h"
#include <d3d11_1.h>


class ComputeShader : public BaseResource
{
public:
	ComputeShader(std::string inName) : BaseResource(inName) {}
	~ComputeShader() { CleanUp(); }

	void InitFromFile(std::string inFileName);
	ID3D11ComputeShader* GetHandle() { return mHandle; }

	void CleanUp();
	ResourceType GetResourceType() const { return ResourceType::COMPUTE_SHADER; };

private:
	ID3D11ComputeShader* mHandle = nullptr;
};

