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

	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, point_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inNormal, point_sampler, 1);
	theRenderContext.CSSetTextureAndSampler(inLinearDepth, point_sampler, 2);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	pCamera cam = Gaag.GetCamera();
	mConstantBufferData.viewspaceReconstructionVector.x = tan(0.5f * cam->GetFovX());
	mConstantBufferData.viewspaceReconstructionVector.y = tan(0.5f * cam->GetFovY());
	mConstantBufferData.targetSize.x = (float)theRenderContext.GetWidth();
	mConstantBufferData.targetSize.y = (float)theRenderContext.GetHeight();
	mConstantBufferData.frameData.x = (float)Gaag.GetFrameID();

	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);
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
	mShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/IndirectLightingCompute.hlsl");
	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferData));
	mInitialized = true;
}


void IndirectLightingRenderer::CleanUp()
{
	mShader = nullptr;
	mConstantBuffer = nullptr;

	mInitialized = false;
}
