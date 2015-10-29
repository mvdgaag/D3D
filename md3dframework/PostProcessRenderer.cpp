#include "PostProcessRenderer.h"
#include "RenderContext.h"
#include "Gaag.h"
#include "ComputeShader.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "ConstantBuffer.h"


void PostProcessRenderer::Render(pTexture inSource, pTexture inMotionVectors, pRenderTarget inTarget)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, Gaag.GetLinearSampler(), 0);
	theRenderContext.CSSetTextureAndSampler(inMotionVectors, Gaag.GetLinearSampler(), 1);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	mConstantBufferData.mTargetSize = float4(theRenderContext.GetWidth(), theRenderContext.GetHeight(),0,0);
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
	theRenderContext.CSSetRWTexture(NULL, 0);
}


void PostProcessRenderer::Init()
{
	CleanUp();
	
	mShader = std::make_shared<ComputeShader>();
	mShader->InitFromFile("../md3dFramework/Shaders/PostProcessCompute.hlsl");

	mConstantBuffer = std::make_shared<ConstantBuffer>();
	mConstantBuffer->Init(sizeof(ConstantBufferData));

	mInitialized = true;
}


void PostProcessRenderer::CleanUp()
{
	mShader = nullptr;
	mConstantBuffer = nullptr;
	mInitialized = false;
}


