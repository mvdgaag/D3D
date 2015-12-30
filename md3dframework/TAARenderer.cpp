#include "TAARenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "ConstantBuffer.h"


void TAARenderer::Render(pTexture inSource, pRenderTarget inHistory, pTexture inMotionVectors, pRenderTarget inTarget)
{
	assert(mInitialized == true);

	pSampler linear_sampler = theResourceFactory.GetDefaultLinearSampler();
	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();

	assert(inSource != nullptr);
	assert(inMotionVectors != nullptr);
	assert(inHistory != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, point_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inHistory->GetTexture(), linear_sampler, 1);
	theRenderContext.CSSetTextureAndSampler(inMotionVectors, point_sampler, 2);
	theRenderContext.CSSetRWTexture(inTarget, 0);
	
	mConstantBufferData.mJitterOffset = GetJitterOffset(Gaag.GetFrameID());
	mConstantBufferData.mTargetSize = float2(theRenderContext.GetWidth(), theRenderContext.GetHeight());
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

	// make history copy;
	Gaag.CopyToRenderTarget(inHistory, inTarget->GetTexture());
}


void TAARenderer::Init()
{
	assert(mInitialized == false);
	mShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/TemporalAACompute.hlsl");
	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferData));
	mInitialized = true;
}


void TAARenderer::CleanUp()
{
	theResourceFactory.DestroyItem(mShader);
	theResourceFactory.DestroyItem(mConstantBuffer);
	mShader = nullptr;
	mConstantBuffer = nullptr;
	mInitialized = false;
}


float2 TAARenderer::GetJitterOffset(int inFrameID)
{
	const float2 halton23[8] = {float2(1.0/2.0, 1.0/3.0),
								float2(1.0/4.0, 2.0/3.0), 
								float2(3.0/4.0, 1.0/9.0), 
								float2(1.0/8.0, 4.0/9.0), 
								float2(5.0/8.0, 7.0/9.0), 
								float2(3.0/8.0, 2.0/9.0), 
								float2(7.0/8.0, 5.0/9.0), 
								float2(1.0/16.0, 8.0/9.0) };
	int idx = inFrameID % 8;
	return float2(halton23[idx] * 2.0f - 1.0f) / float2(theRenderContext.GetWidth(), theRenderContext.GetHeight());
}