#include "IndirectLightingRenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "Camera.h"
#include "ConstantBuffer.h"


void IndirectLightingRenderer::Render(pTexture inSource, pTexture inNormal, pTexture inLinearDepth, pRenderTarget inTarget)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, Gaag.GetPointSampler(), 0);
	theRenderContext.CSSetTextureAndSampler(inNormal, Gaag.GetPointSampler(), 1);
	theRenderContext.CSSetTextureAndSampler(inLinearDepth, Gaag.GetPointSampler(), 2);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	pCamera cam = Gaag.GetCamera();
	mConstantBufferData.viewspaceReconstructionVector.x = tan(0.5 * cam->GetFovX());
	mConstantBufferData.viewspaceReconstructionVector.y = tan(0.5 * cam->GetFovY());
	mConstantBufferData.targetSize.x = theRenderContext.GetWidth();
	mConstantBufferData.targetSize.y = theRenderContext.GetHeight();
	mConstantBufferData.frameData.x = Gaag.GetFrameID();

	theRenderContext.UpdateSubResource(mConstantBuffer, &mConstantBufferData);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	int groups_x = (inTarget->GetTexture()->GetWidth() + 7) / 8;
	int groups_y = (inTarget->GetTexture()->GetHeight() + 7) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetRWTexture(NULL, 0);
}


void IndirectLightingRenderer::Init()
{
	CleanUp();
	mShader = std::make_shared<ComputeShader>();
	mShader->InitFromFile("../md3dFramework/Shaders/IndirectLightingCompute.hlsl");
	mConstantBuffer = std::make_shared<ConstantBuffer>();
	mConstantBuffer->Init(sizeof(ConstantBufferData));
	mInitialized = true;
}


void IndirectLightingRenderer::CleanUp()
{
	mShader = nullptr;
	mConstantBuffer = nullptr;

	mInitialized = false;
}
