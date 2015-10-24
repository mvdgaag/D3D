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


void DirectLightingRenderer::Render(GBuffer* inSource, RenderTarget* inTargetDiffuse, RenderTarget* inTargetSpecular)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTargetDiffuse != nullptr);
	assert(inTargetSpecular != nullptr);
	assert(inTargetDiffuse->GetTexture()->GetWidth() == inTargetDiffuse->GetTexture()->GetWidth() &&
		inTargetDiffuse->GetTexture()->GetHeight() == inTargetDiffuse->GetTexture()->GetHeight());

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::LINEAR_DEPTH), theFramework.GetPointSampler(), 0);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::NORMAL), theFramework.GetPointSampler(), 1);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::DIFFUSE), theFramework.GetPointSampler(), 2);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::MATERIAL), theFramework.GetPointSampler(), 3);
	theRenderContext.CSSetRWTexture(inTargetDiffuse, 0);
	theRenderContext.CSSetRWTexture(inTargetSpecular, 1);

	Camera* cam = theFramework.GetCamera();
	mConstantBufferData.viewspaceReconstructionVector.x = tan(0.5 * cam->GetFovX());
	mConstantBufferData.viewspaceReconstructionVector.y = tan(0.5 * cam->GetFovY());
	mConstantBufferData.targetSize.x = theRenderContext.GetWidth();
	mConstantBufferData.targetSize.y = theRenderContext.GetHeight();
	mConstantBufferData.frameData.x = theFramework.GetFrameID();

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
	mShader = new ComputeShader();
	mShader->InitFromFile("Shaders/DirectLightingCompute.hlsl");
	mConstantBuffer = new ConstantBuffer();
	mConstantBuffer->Init(sizeof(ConstantBufferData));
	mInitialized = true;
}


void DirectLightingRenderer::CleanUp()
{
	delete mShader;
	mShader = nullptr;
	
	delete mConstantBuffer;
	mConstantBuffer = nullptr;
	
	mInitialized = false;
}
