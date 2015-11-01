#include "ReflectionRenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"


void ReflectionRenderer::Render(pTexture inSource, pRenderTarget inTarget)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, Gaag.GetPointSampler(), 0);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	int groups_x = (inTarget->GetTexture()->GetWidth() + 7) / 8;
	int groups_y = (inTarget->GetTexture()->GetHeight() + 7) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
}


void ReflectionRenderer::Init()
{
	CleanUp();
	mShader = MAKE_NEW(ComputeShader);
	mShader->InitFromFile("../md3dFramework/Shaders/ReflectionCompute.hlsl");
	mInitialized = true;
}


void ReflectionRenderer::CleanUp()
{
	mShader = nullptr;
	mInitialized = false;
}

