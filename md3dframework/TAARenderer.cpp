#include "TAARenderer.h"
#include "RenderContext.h"
#include "Framework.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"


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

	int groups_x = (inTarget->GetTexture()->GetWidth() + 7) / 8;
	int groups_y = (inTarget->GetTexture()->GetHeight() + 7) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);

	// TODO: required?
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
	mInitialized = true;
}


void TAARenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
	mInitialized = false;
}
