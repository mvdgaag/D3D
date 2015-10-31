#include "DirectLightingRenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "GBuffer.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include <assert.h>


void DirectLightingRenderer::Render(pGBuffer inSource, pRenderTarget inTargetDiffuse, pRenderTarget inTargetSpecular)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTargetDiffuse != nullptr);
	assert(inTargetSpecular != nullptr);
	assert(inTargetDiffuse->GetTexture()->GetWidth() == inTargetDiffuse->GetTexture()->GetWidth() &&
		inTargetDiffuse->GetTexture()->GetHeight() == inTargetDiffuse->GetTexture()->GetHeight());

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::LINEAR_DEPTH), Gaag.GetPointSampler(), 0);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::NORMAL), Gaag.GetPointSampler(), 1);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::DIFFUSE), Gaag.GetPointSampler(), 2);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::MATERIAL), Gaag.GetPointSampler(), 3);
	theRenderContext.CSSetRWTexture(inTargetDiffuse, 0);
	theRenderContext.CSSetRWTexture(inTargetSpecular, 1);

	pCamera cam = Gaag.GetCamera();
	mConstantBufferData.viewspaceReconstructionVector.x = tan(0.5 * cam->GetFovX());
	mConstantBufferData.viewspaceReconstructionVector.y = tan(0.5 * cam->GetFovY());
	mConstantBufferData.targetSize.x = theRenderContext.GetWidth();
	mConstantBufferData.targetSize.y = theRenderContext.GetHeight();
	mConstantBufferData.frameData.x = Gaag.GetFrameID();

	theRenderContext.UpdateSubResource(mConstantBuffer, &mConstantBufferData);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	int groups_x = (inTargetDiffuse->GetTexture()->GetWidth() + 7) / 8;
	int groups_y = (inTargetDiffuse->GetTexture()->GetHeight() + 7) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 3);
	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 1);
}


void DirectLightingRenderer::Init()
{
	CleanUp();
	mShader = std::make_shared<ComputeShader>();
	mShader->InitFromFile("../md3dFramework/Shaders/DirectLightingCompute.hlsl");
	mConstantBuffer = std::make_shared<ConstantBuffer>();
	mConstantBuffer->Init(sizeof(ConstantBufferData));
	mInitialized = true;
}


void DirectLightingRenderer::CleanUp()
{
	mShader = nullptr;
	mConstantBuffer = nullptr;
	mInitialized = false;
}
