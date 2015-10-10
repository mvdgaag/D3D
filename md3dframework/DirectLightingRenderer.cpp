#include "DirectLightingRenderer.h"
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

	theFramework.SetComputeShader(mShader);
	theFramework.ComputeSetTextureAndSampler(inSource->GetTexture(GBuffer::LINEAR_DEPTH), theFramework.GetPointSampler(), 0);
	theFramework.ComputeSetTextureAndSampler(inSource->GetTexture(GBuffer::NORMAL), theFramework.GetPointSampler(), 1);
	theFramework.ComputeSetTextureAndSampler(inSource->GetTexture(GBuffer::DIFFUSE), theFramework.GetPointSampler(), 2);
	theFramework.ComputeSetTextureAndSampler(inSource->GetTexture(GBuffer::MATERIAL), theFramework.GetPointSampler(), 3);
	theFramework.ComputeSetRWTexture(inTarget, 0);

	// TODO: test:
	Camera* cam = theFramework.GetCamera();
	mConstantBufferData.viewspaceReconstructionVector.x = cam->GetNear() / tan(0.5 * cam->GetFovX());
	mConstantBufferData.viewspaceReconstructionVector.y = cam->GetNear() / tan(0.5 * cam->GetFovY());
	mConstantBufferData.targetSize.x = theFramework.GetScreenWidth();
	mConstantBufferData.targetSize.y = theFramework.GetScreenHeight();
	mConstantBufferData.frameData.x = theFramework.GetFrameID();

	//mConstantBuffer->SetData(&mConstantBufferData);
	ID3D11Buffer* cbuf = mConstantBuffer->GetBuffer();
	ID3D11DeviceContext* context;
	theFramework.GetDevice()->GetImmediateContext(&context);
	context->UpdateSubresource(cbuf, 0, NULL, &mConstantBufferData, 0, 0);
	theFramework.ComputeSetConstantBuffer(mConstantBuffer);

	int groups_x = 1 + (inTarget->GetTexture()->GetWidth() - 1) / 8;
	int groups_y = 1 + (inTarget->GetTexture()->GetHeight() - 1) / 8;
	theFramework.ComputeDispatch(groups_x, groups_y, 1);

	// TODO: required?
	theFramework.Flush();

	// clear state
	theFramework.SetComputeShader(NULL);
	theFramework.ComputeSetTextureAndSampler(NULL, NULL, 0);
	theFramework.ComputeSetTextureAndSampler(NULL, NULL, 1);
	theFramework.ComputeSetTextureAndSampler(NULL, NULL, 2);
	theFramework.ComputeSetTextureAndSampler(NULL, NULL, 3);
	theFramework.ComputeSetRWTexture(NULL, 0);
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