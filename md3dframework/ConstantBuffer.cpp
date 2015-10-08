#include "ConstantBuffer.h"
#include "main.h"
#include "Framework.h"


void ConstantBuffer::Init(int inByteWidth)
{
	assert(inByteWidth % 16 == 0);
	ZeroMemory(&mDesc, sizeof(mDesc));
	mDesc.Usage = D3D11_USAGE_DEFAULT;
	mDesc.ByteWidth = inByteWidth;
	mDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mDesc.CPUAccessFlags = 0;

	D3DCall(theFramework.GetDevice()->CreateBuffer(&mDesc, nullptr, &mBuffer));
}


void ConstantBuffer::SetData(const void* inData)
{
	ID3D11DeviceContext* context;
	theFramework.GetDevice()->GetImmediateContext(&context);
	context->UpdateSubresource(mBuffer, 0, nullptr, &inData, 0, 0);
}


void ConstantBuffer::CleanUp()
{
	if (mBuffer != nullptr)
	{
		mBuffer->Release();
		mBuffer = nullptr;
	}
}
