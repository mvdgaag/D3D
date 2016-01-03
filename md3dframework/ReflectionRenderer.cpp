#include "ReflectionRenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"


void ReflectionRenderer::Render(pTexture inSource, pRenderTarget inTarget)
{
	assert(mInitialized == true);

	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, point_sampler, 0);
	theRenderContext.CSSetRWTexture(inTarget, 0, 0);

	int2 groups = (inTarget->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(groups.x, groups.y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
}


void ReflectionRenderer::Init()
{
	assert(mInitialized == false);
	mShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/ReflectionCompute.hlsl");
	mInitialized = true;
}


void ReflectionRenderer::CleanUp()
{
	theResourceFactory.DestroyItem(mShader);
	mShader = nullptr;
	mInitialized = false;
}

