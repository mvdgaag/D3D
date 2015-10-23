#pragma once
#include "BaseResource.h"

struct ID3D11Buffer;
struct D3D11_BUFFER_DESC;

class ConstantBuffer : public BaseResource
{
	friend class RenderContext;

public:
	ConstantBuffer() : BaseResource() {}
	~ConstantBuffer() { CleanUp(); }

	void Init(int inByteWidth);
	void CleanUp() override;

	ResourceType GetResourceType() const override { return ResourceType::CONSTANT_BUFFER; };

protected:
	ID3D11Buffer* mBuffer;
	D3D11_BUFFER_DESC* mDesc;
};

