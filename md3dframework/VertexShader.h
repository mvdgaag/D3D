#pragma once
#include "BaseResource.h"

struct ID3D11VertexShader;
struct ID3D11InputLayout;

class VertexShader : public BaseResource
{
	friend class RenderContext;

public:
	VertexShader() : BaseResource() {}
	~VertexShader() { CleanUp(); }

	void InitFromFile(std::string inFileName);
	void CleanUp();
	ResourceType GetResourceType() const { return ResourceType::VERTEX_SHADER; };

private:
	ID3D11VertexShader* mHandle = nullptr;
	ID3D11InputLayout*	mVertexLayout = nullptr;
};

