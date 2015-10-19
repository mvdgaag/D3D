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


void DrawableObject::Draw()
{
	theFramework.SetMaterial(mMaterial);

	mConstantBufferData.offsets.z = mConstantBufferData.offsets.x; // previous x
	mConstantBufferData.offsets.w = mConstantBufferData.offsets.y; // previous y
	float2 jitter_offset = TAARenderer::GetJitterOffset(theFramework.GetFrameID());
	mConstantBufferData.offsets.x = jitter_offset.x;
	mConstantBufferData.offsets.y = jitter_offset.y;
	mConstantBufferData.prevMVP = mConstantBufferData.MVP;
	mConstantBufferData.MVP = DirectX::XMMatrixTranspose(theFramework.GetCamera()->GetViewProjectionMatrix());
	
	mTransform = DirectX::XMMatrixTranslation(sin(theFramework.GetFrameID() / 100.0f), 0.0, 0.0);
	// TODO: this line keeps giving unhandled exceptions: solution: put a break point and step into: MAGIC?!?
	mConstantBufferData.MVP *= DirectX::XMMatrixTranspose(mTransform);

	// set constant buffers
	theRenderContext.UpdateSubResource(mConstantBuffer, &mConstantBufferData);
	theRenderContext.VSSetConstantBuffer(mConstantBuffer, 0);
	theRenderContext.DrawMesh(mMesh);

	// reset state
	// TODO: move to material and mesh classes?
	theRenderContext.VSSetConstantBuffer(NULL, 0);
	theRenderContext.PSSetConstantBuffer(NULL, 0);
	theRenderContext.PSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.PSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.PSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.PSSetShader(NULL);
	theRenderContext.VSSetShader(NULL);
}