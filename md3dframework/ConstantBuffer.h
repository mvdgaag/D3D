#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"

struct ID3D11Buffer;
struct D3D11_BUFFER_DESC;

REGISTERCLASS(ConstantBuffer);

class ConstantBuffer : public BaseResource
{
	friend class RenderContext;
	friend class ResourceFactory;

public:
	void Init(int inByteWidth);
	void CleanUp() override;

	ResourceType GetResourceType() const override { return ResourceType::CONSTANT_BUFFER; }

protected:
	ID3D11Buffer* mBuffer = nullptr;
	D3D11_BUFFER_DESC* mDesc = nullptr;

private:
	ConstantBuffer() : BaseResource() {}
	~ConstantBuffer() { CleanUp(); }

	ConstantBuffer(ConstantBuffer const&) = delete;
	void operator=(ConstantBuffer const&) = delete;
};

