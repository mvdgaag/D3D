#include "DirectLightingRenderer.h"
#include "RenderContext.h"
#include "Framework.h"
#include "ComputeShader.h"
#include "GBuffer.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include <assert.h>


void DirectLightingRenderer::Render(GBuffer* inSource, RenderTarget* inTarget)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::LINEAR_DEPTH), theFramework.GetPointSampler(), 0);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::NORMAL), theFramework.GetPointSampler(), 1);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::DIFFUSE), theFramework.GetPointSampler(), 2);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::MATERIAL), theFramework.GetPointSampler(), 3);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	Camera* cam = theFramework.GetCamera();
	mConstantBufferData.viewspaceReconstructionVector.x = cam->GetNear() / tan(0.5 * cam->GetFovX());
	mConstantBufferData.viewspaceReconstructionVector.y = cam->GetNear() / tan(0.5 * cam->GetFovY());
	mConstantBufferData.targetSize.x = theRenderContext.GetWidth();
	mConstantBufferData.targetSize.y = theRenderContext.GetHeight();
	mConstantBufferData.frameData.x = theFramework.GetFrameID();

	//mConstantBuffer->SetData(&mConstantBufferData);
	ID3D11Buffer* cbuf = mConstantBuffer->GetBuffer();
	ID3D11DeviceContext* context;
	theRenderContext.GetDevice()->GetImmediateContext(&context);
	context->UpdateSubresource(cbuf, 0, NULL, &mConstantBufferData, 0, 0);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	int groups_x = 1 + (inTarget->GetTexture()->GetWidth() - 1) / 8;
	int groups_y = 1 + (inTarget->GetTexture()->GetHeight() - 1) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);

	// TODO: required?
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 3);
	theRenderContext.CSSetRWTexture(NULL, 0);
}


void DirectLightingRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader();
	mShader->InitFromFile("Shaders/DirectLightingCompute.hlsl");
	mConstantBuffer = new ConstantBuffer();
	mConstantBuffer->Init(sizeof(ConstantBufferData));
	mInitialized = true;
}


void DirectLightingRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
	if (mConstantBuffer)
	{
		delete mConstantBuffer;
	}
	mInitialized = false;
}