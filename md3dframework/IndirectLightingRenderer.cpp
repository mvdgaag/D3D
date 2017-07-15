#include "IndirectLightingRenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "TextureUtil.h"


void IndirectLightingRenderer::Render(pTexture inSource, pTexture inNormal, pTexture inLinearDepth, pTexture inMaxDepth, pTexture inDiffuse, pRenderTarget inTarget, pRenderTarget inTempTarget)
{
	assert(mInitialized == true);
	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	ApplyIndirect(inSource, inNormal, inLinearDepth, inMaxDepth, inDiffuse, inTarget);

	for (int i = 0; i < 2; i++)
	{
		ApplyBlur(inTarget->GetTexture(), inNormal, inLinearDepth, inTempTarget, true);
		ApplyBlur(inTempTarget->GetTexture(), inNormal, inLinearDepth, inTarget, false);
	}
}


void IndirectLightingRenderer::ApplyBlur(pTexture inSource, pTexture inNormal, pTexture inLinearDepth, pRenderTarget inTarget, bool inHorizontal)
{
	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();
	pSampler linear_sampler = theResourceFactory.GetDefaultLinearSampler();

	theRenderContext.CSSetShader(mBlurShader);
	theRenderContext.CSSetTextureAndSampler(inSource, linear_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inNormal, point_sampler, 1);
	theRenderContext.CSSetTextureAndSampler(inLinearDepth, point_sampler, 2);
	theRenderContext.CSSetRWTexture(inTarget, 0, 0);

	pCamera cam = Gaag.GetCamera();
	mConstantBufferData.viewspaceReconstructionVector.x = 1.0f / cam->GetProjectionMatrix()[0][0];
	mConstantBufferData.viewspaceReconstructionVector.y = 1.0f / cam->GetProjectionMatrix()[1][1];
	mConstantBufferData.targetSize.x = (float)theRenderContext.GetWidth();
	mConstantBufferData.targetSize.y = (float)theRenderContext.GetHeight();
	mConstantBufferData.frameData.x = inHorizontal ? 1.0f : 0.0f;
	mConstantBufferData.frameData.y = 4; // pattern has a radius of 4 pixels, this radius attempts to remove the pattern

	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	int2 groups = (inTarget->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(groups.x, groups.y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
}


void IndirectLightingRenderer::ApplyIndirect(pTexture inSource, pTexture inNormal, pTexture inLinearDepth, pTexture inMaxDepth, pTexture inDiffuse, pRenderTarget inTarget)
{
	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();

	theRenderContext.CSSetShader(mLightingShader);
	theRenderContext.CSSetTextureAndSampler(inSource, point_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inNormal, point_sampler, 1);
	theRenderContext.CSSetTextureAndSampler(inLinearDepth, point_sampler, 2);
	theRenderContext.CSSetTextureAndSampler(inDiffuse, point_sampler, 3);
	theRenderContext.CSSetTextureAndSampler(inMaxDepth, point_sampler, 4);
	theRenderContext.CSSetRWTexture(inTarget, 0, 0);

	pCamera cam = Gaag.GetCamera();
	mConstantBufferData.viewspaceReconstructionVector.x = 1.0f / cam->GetProjectionMatrix()[0][0];
	mConstantBufferData.viewspaceReconstructionVector.y = 1.0f / cam->GetProjectionMatrix()[1][1];
	mConstantBufferData.targetSize = inTarget->GetDimensions();
	mConstantBufferData.frameData = float4(Gaag.WorldToCameraNormal(float3(0.0, 1.0, 0.0)), Gaag.GetRandom());

	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	int2 groups = (inTarget->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(groups.x, groups.y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 3);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 4);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
}


void IndirectLightingRenderer::Init()
{
	assert(mInitialized == false);
	mLightingShader = theResourceFactory.LoadComputeShader(Gaag.GetDirectory("/Shaders/IndirectLightingCompute.hlsl"));
	mBlurShader = theResourceFactory.LoadComputeShader(Gaag.GetDirectory("/Shaders/IndirectBlurCompute.hlsl"));
	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferData));
	mInitialized = true;
}


void IndirectLightingRenderer::CleanUp()
{
	if (mInitialized == false)
		return;

	theResourceFactory.DestroyItem(mLightingShader);
	theResourceFactory.DestroyItem(mBlurShader);
	theResourceFactory.DestroyItem(mConstantBuffer);
	mLightingShader = nullptr;
	mBlurShader = nullptr;
	mConstantBuffer = nullptr;
	mInitialized = false;
}
