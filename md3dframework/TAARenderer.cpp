#include "TAARenderer.h"
#include "RenderContext.h"
#include "Framework.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "ConstantBuffer.h"


void TAARenderer::Render(Texture* inSource, RenderTarget* inHistory, Texture* inMotionVectors, RenderTarget* inTarget)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inMotionVectors != nullptr);
	assert(inHistory != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, theFramework.GetPointSampler(), 0);
	theRenderContext.CSSetTextureAndSampler(inHistory->GetTexture(), theFramework.GetLinearSampler(), 1);
	theRenderContext.CSSetTextureAndSampler(inMotionVectors, theFramework.GetPointSampler(), 2);
	theRenderContext.CSSetRWTexture(inTarget, 0);
	
	mConstantBufferData.mJitterOffset = GetJitterOffset(theFramework.GetFrameID());
	mConstantBufferData.mTargetSize = float2(theRenderContext.GetWidth(), theRenderContext.GetHeight());
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

	// make history copy;
	theFramework.CopyToRenderTarget(inHistory, inTarget->GetTexture());
}


void TAARenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader();
	mShader->InitFromFile("Shaders/TemporalAACompute.hlsl");
	mConstantBuffer = new ConstantBuffer();
	mConstantBuffer->Init(sizeof(ConstantBufferData));
	mInitialized = true;
}


void TAARenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
	if (mConstantBuffer)
	{
		delete mConstantBuffer;
		mConstantBuffer = NULL;
	}
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