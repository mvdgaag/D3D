#include "PostProcessRenderer.h"
#include "RenderContext.h"
#include "Framework.h"
#include "ComputeShader.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "ConstantBuffer.h"


void PostProcessRenderer::Render(Texture* inSource, Texture* inMotionVectors, RenderTarget* inTarget)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, theFramework.GetLinearSampler(), 0);
	theRenderContext.CSSetTextureAndSampler(inMotionVectors, theFramework.GetLinearSampler(), 1);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	mConstantBufferData.mTargetSize = float4(theRenderContext.GetWidth(), theRenderContext.GetHeight(),0,0);
	theRenderContext.UpdateSubResource(mConstantBuffer, &mConstantBufferData);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	int groups_x = (inTarget->GetTexture()->GetWidth() + 7) / 8;
	int groups_y = (inTarget->GetTexture()->GetHeight() + 7) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);

	// TODO: required?
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetRWTexture(NULL, 0);
}


void PostProcessRenderer::Init()
{
	CleanUp();
	
	mShader = new ComputeShader();
	mShader->InitFromFile("Shaders/PostProcessCompute.hlsl");

	mConstantBuffer = new ConstantBuffer();
	mConstantBuffer->Init(sizeof(ConstantBufferData));

	mInitialized = true;
}


void PostProcessRenderer::CleanUp()
{
	delete mShader;
	mShader = nullptr;
	
	delete mConstantBuffer;
	mConstantBuffer = nullptr;

	mInitialized = false;
}


