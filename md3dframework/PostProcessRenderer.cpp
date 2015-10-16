#include "PostProcessRenderer.h"
#include "RenderContext.h"
#include "Framework.h"
#include "ComputeShader.h"
#include "RenderTarget.h"
#include "Texture.h"


void PostProcessRenderer::Render(Texture* inSource, RenderTarget* inTarget)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource, theFramework.GetPointSampler(), 0);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	int groups_x = (inTarget->GetTexture()->GetWidth() + 7) / 8;
	int groups_y = (inTarget->GetTexture()->GetHeight() + 7) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);

	// TODO: required?
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
}


void PostProcessRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader();
	mShader->InitFromFile("Shaders/PostProcessCompute.hlsl");
	mInitialized = true;
}


void PostProcessRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
	mInitialized = false;
}


