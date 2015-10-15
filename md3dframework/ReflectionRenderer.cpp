#include "ReflectionRenderer.h"
#include "RenderContext.h"
#include "Framework.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"


void ReflectionRenderer::Render(Texture* inSource, RenderTarget* inTarget)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, theFramework.GetPointSampler(), 0);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	int groups_x = 1 + (inTarget->GetTexture()->GetWidth() - 1) / 8;
	int groups_y = 1 + (inTarget->GetTexture()->GetHeight() - 1) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);

	// TODO: required?
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
}


void ReflectionRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader();
	mShader->InitFromFile("Shaders/ReflectionCompute.hlsl");
	mInitialized = true;
}


void ReflectionRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
	mInitialized = false;
}

