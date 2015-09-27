#include "ConstantBuffer.h"
#include "main.h"
#include "FrameWork.h"


void ConstantBuffer::Init(int inByteWidth)
{
	ZeroMemory(&mDesc, sizeof(mDesc));
	mDesc.Usage = D3D11_USAGE_DEFAULT;
	mDesc.ByteWidth = inByteWidth;
	mDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mDesc.CPUAccessFlags = 0;

	D3DCall(theFrameWork.GetDevice()->CreateBuffer(&mDesc, nullptr, &mBuffer));
}


void ConstantBuffer::CleanUp()
{
	if (mBuffer != nullptr)
	{
		mBuffer->Release();
		mBuffer = nullptr;
	}
}
