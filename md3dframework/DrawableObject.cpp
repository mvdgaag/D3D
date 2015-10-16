#include "DrawableObject.h"
#include "RenderContext.h"
#include "Framework.h"
#include "Camera.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include <assert.h>


void DrawableObject::Init(Mesh* inMesh, Material* inMaterial)
{
	mMesh = inMesh;
	mMaterial = inMaterial;
	mConstantBuffer = new ConstantBuffer();
	mConstantBuffer->Init(sizeof(ConstantBufferData));
	mTransform = DirectX::XMMatrixIdentity();
}


void DrawableObject::Draw()
{
	float val = mMaterial->GetRoughnessValue();
	val = val < 0.99 ? val + 0.01 : 0.0;
	mMaterial->SetRoughnessValue(val);

	theFramework.SetMaterial(mMaterial);

	const float halton23x[8] = {	1.0f/2.0f, 1.0f/4.0f, 3.0f/4.0f, 1.0f/8.0f,
									5.0f/8.0f, 3.0f/8.0f, 7.0f/8.0f, 1.0f/16.0f };
	const float halton23y[8] = {	1.0f/3.0f, 2.0f/3.0f, 1.0f/9.0f, 4.0f/9.0f,
									7.0f/9.0f, 2.0f/9.0f, 5.0f/9.0f, 8.0f/9.0f };
	int idx = theFramework.GetFrameID() % 8;
	mConstantBufferData.offsets.z = mConstantBufferData.offsets.x; // previous x
	mConstantBufferData.offsets.w = mConstantBufferData.offsets.y; // previous y
	mConstantBufferData.offsets.x = ((halton23x[idx] * 2.0f - 1.0f) / theRenderContext.GetWidth());
	mConstantBufferData.offsets.y = ((halton23y[idx] * 2.0f - 1.0f) / theRenderContext.GetHeight());
	mConstantBufferData.prevMVP = mConstantBufferData.MVP;
	mConstantBufferData.MVP = theFramework.GetCamera()->GetProjectionMatrix() * theFramework.GetCamera()->GetViewMatrix();

	// set constant buffers
	theRenderContext.UpdateSubResource(mConstantBuffer, &mConstantBufferData);
	theRenderContext.VSSetConstantBuffer(mConstantBuffer, 0);
	theRenderContext.DrawMesh(mMesh);

	// reset state
	theRenderContext.PSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.PSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.PSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.PSSetShader(NULL);
	theRenderContext.VSSetShader(NULL);
}