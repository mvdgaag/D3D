#include "LightComposeRenderer.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "RenderContext.h"
#include "Texture.h"
#include "RenderTarget.h"


void LightComposeRenderer::Render(pTexture inDirectDiffuse, pTexture inDirectSpecular, pTexture inIndirect, 
	pTexture inReflections, pTexture inDepth, pTexture inHalfDepth, pRenderTarget inTarget)
{
	assert(mInitialized == true);

	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();

	assert(inDirectDiffuse != nullptr);
	assert(inDirectSpecular != nullptr);
	assert(inIndirect != nullptr);
	assert(inReflections != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inDirectDiffuse, point_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inDirectSpecular, point_sampler, 1);
	theRenderContext.CSSetTextureAndSampler(inIndirect, point_sampler, 2);
	theRenderContext.CSSetTextureAndSampler(inReflections, point_sampler, 3);
	theRenderContext.CSSetTextureAndSampler(inDepth, point_sampler, 4);
	theRenderContext.CSSetTextureAndSampler(inHalfDepth, point_sampler, 5);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	int2 groups = (inTarget->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(groups.x, groups.y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 3);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 4);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 5);
	theRenderContext.CSSetRWTexture(NULL, 0);
}


void LightComposeRenderer::Init()
{
	assert(mInitialized == false);
	mShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/LightComposeCompute.hlsl");
	mInitialized = true;
}


void LightComposeRenderer::CleanUp()
{
	theResourceFactory.DestroyItem(mShader);
	mShader = nullptr;
	mInitialized = false;
}
