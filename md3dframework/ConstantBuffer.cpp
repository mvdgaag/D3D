#include "ConstantBuffer.h"
#include "RenderContext.h"
#include <d3d11_1.h>


void ConstantBuffer::Init(int inByteWidth)
{
	CleanUp();

	assert(inByteWidth % 16 == 0);

	mDesc = new D3D11_BUFFER_DESC();
	ZeroMemory(mDesc, sizeof(D3D11_BUFFER_DESC));
	mDesc->Usage = D3D11_USAGE_DEFAULT;
	mDesc->ByteWidth = inByteWidth;
	mDesc->BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mDesc->CPUAccessFlags = 0;

	D3DCall(theRenderContext.GetDevice()->CreateBuffer(mDesc, nullptr, &mBuffer));
}


void ConstantBuffer::CleanUp()
{
	if (mBuffer != nullptr)
	{
		mBuffer->Release();
		mBuffer = nullptr;
	}
	if (mDesc != nullptr)
		delete mDesc;
}
