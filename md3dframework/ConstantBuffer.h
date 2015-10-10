#pragma once
#include "BaseResource.h"
#include <d3d11_1.h>

class ConstantBuffer : public BaseResource
{
public:
	ConstantBuffer() : BaseResource() {}
	~ConstantBuffer() { CleanUp(); }

	void Init(int inByteWidth);

	ID3D11Buffer* GetBuffer() { return mBuffer; }
	D3D11_BUFFER_DESC GetDescriptor() { return mDesc; }
	void SetData(const void* inData);

	void CleanUp();
	ResourceType GetResourceType() const { return ResourceType::CONSTANT_BUFFER; };

private:
	ID3D11Buffer* mBuffer;
	D3D11_BUFFER_DESC mDesc;
};

