#include "DrawableObject.h"
#include "RenderContext.h"
#include "Framework.h"
#include "Camera.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "TAARenderer.h"
#include <assert.h>


void DrawableObject::Init(Mesh* inMesh, Material* inMaterial)
{
	mMesh = inMesh;
	mMaterial = inMaterial;
	mConstantBuffer = new ConstantBuffer();
	mConstantBuffer->Init(sizeof(ConstantBufferData));
	mTransform = DirectX::XMMatrixIdentity();
}


void DrawableObject::CleanUp()
{
	// TODO: does this object own the mesh, material and buffer?
	delete mMesh;
	mMesh = nullptr;

	delete mMaterial;
	mMaterial = nullptr;

	delete mConstantBuffer;
	mConstantBuffer = nullptr;
}


ConstantBuffer* DrawableObject::GetConstantBuffer()
{
	// TODO: check if already updated this frame?
	mConstantBufferData.modelView = DirectX::XMMatrixTranspose(theFramework.GetCamera()->GetViewMatrix());
	mConstantBufferData.modelView *= DirectX::XMMatrixTranspose(mTransform);
	mConstantBufferData.prevModelViewProjection = mConstantBufferData.modelViewProjection;
	mConstantBufferData.modelViewProjection = DirectX::XMMatrixTranspose(theFramework.GetCamera()->GetViewProjectionMatrix());
	mConstantBufferData.modelViewProjection *= DirectX::XMMatrixTranspose(mTransform);
	theRenderContext.UpdateSubResource(mConstantBuffer, &mConstantBufferData);
	return mConstantBuffer;
}
