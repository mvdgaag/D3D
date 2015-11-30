#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"

struct ID3D11VertexShader;
struct ID3D11InputLayout;

REGISTERCLASS(VertexShader);

class VertexShader : public BaseResource
{
	friend class ResourceFactory;
	friend class RenderContext;

public:
	void InitFromFile(std::string inFileName);
	void CleanUp() override;
	ResourceType GetResourceType() const override { return ResourceType::VERTEX_SHADER; };

protected:
	ID3D11VertexShader* mHandle = nullptr;
	ID3D11InputLayout*	mVertexLayout = nullptr;

private:
	VertexShader() : BaseResource() {}
	~VertexShader() { CleanUp(); }

	VertexShader(VertexShader const&) = delete;
	void operator=(VertexShader const&) = delete;
};
