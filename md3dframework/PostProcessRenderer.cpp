#include "PostProcessRenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "ConstantBuffer.h"


void PostProcessRenderer::Render(pTexture inSource, pTexture inMotionVectors, pRenderTarget inTarget)
{
	assert(mInitialized == true);

	pSampler linear_sampler = theResourceFactory.GetDefaultLinearSampler();

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, linear_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inMotionVectors, linear_sampler, 1);
	theRenderContext.CSSetRWTexture(inTarget, 0, 0);

	mConstantBufferData.mTargetSize = float4(theRenderContext.GetWidth(), theRenderContext.GetHeight(), Gaag.GetFrameID(),0);
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	int2 groups = (inTarget->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(groups.x, groups.y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
}


void PostProcessRenderer::Init()
{
	assert(mInitialized == false);
	mShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/PostProcessCompute.hlsl");
	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferData));
	mInitialized = true;
}


void PostProcessRenderer::CleanUp()
{
	theResourceFactory.DestroyItem(mShader);
	theResourceFactory.DestroyItem(mConstantBuffer);
	mShader = nullptr;
	mConstantBuffer = nullptr;
	mInitialized = false;
}


