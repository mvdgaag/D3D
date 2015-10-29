#include "DrawableObject.h"
#include "RenderContext.h"
#include "Gaag.h"
#include "Camera.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "TAARenderer.h"
#include <assert.h>


void DrawableObject::Init(pMesh inMesh, pMaterial inMaterial)
{
	mMesh = inMesh;
	mMaterial = inMaterial;
	mConstantBuffer = std::make_shared<ConstantBuffer>();
	mConstantBuffer->Init(sizeof(ConstantBufferData));
	mTransform = DirectX::XMMatrixIdentity();
}


void DrawableObject::CleanUp()
{
	mMesh = nullptr;
	mMaterial = nullptr;
	mConstantBuffer = nullptr;
}


pConstantBuffer DrawableObject::GetConstantBuffer()
{
	// TODO: check if already updated this frame?
	mConstantBufferData.modelView = DirectX::XMMatrixTranspose(Gaag.GetCamera()->GetViewMatrix());
	mConstantBufferData.modelView *= DirectX::XMMatrixTranspose(mTransform);
	mConstantBufferData.prevModelViewProjection = mConstantBufferData.modelViewProjection;
	mConstantBufferData.modelViewProjection = DirectX::XMMatrixTranspose(Gaag.GetCamera()->GetViewProjectionMatrix());
	mConstantBufferData.modelViewProjection *= DirectX::XMMatrixTranspose(mTransform);
	theRenderContext.UpdateSubResource(mConstantBuffer, &mConstantBufferData);
	return mConstantBuffer;
}
