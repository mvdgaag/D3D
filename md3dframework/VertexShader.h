#pragma once
#include "BaseResource.h"
#include <d3d11_1.h>


class VertexShader : public BaseResource
{
public:
	VertexShader(std::string inName) : BaseResource(inName) {}
	~VertexShader() { CleanUp(); }

	void InitFromFile(std::string inFileName);
	ID3D11VertexShader* GetHandle() { return mHandle; }
	ID3D11InputLayout* GetLayout() { return mVertexLayout; }

	void CleanUp();
	ResourceType GetResourceType() const { return ResourceType::VERTEX_SHADER; };

private:
	ID3D11VertexShader* mHandle = nullptr;
	ID3D11InputLayout*	mVertexLayout = nullptr;
};

