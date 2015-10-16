#include "LightComposeRenderer.h"
#include "Framework.h"
#include "ComputeShader.h"
#include "RenderContext.h"
#include "Texture.h"
#include "RenderTarget.h"


void LightComposeRenderer::Render(Texture* inDirect, Texture* inIndirect, Texture* inReflections, RenderTarget* inTarget)
{
	assert(mInitialized == true);

	assert(inDirect != nullptr);
	assert(inIndirect != nullptr);
	assert(inReflections != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inDirect, theFramework.GetPointSampler(), 0);
	theRenderContext.CSSetTextureAndSampler(inIndirect, theFramework.GetPointSampler(), 1);
	theRenderContext.CSSetTextureAndSampler(inReflections, theFramework.GetPointSampler(), 2);
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
}


void LightComposeRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader();
	mShader->InitFromFile("Shaders/LightComposeCompute.hlsl");
	mInitialized = true;
}


void LightComposeRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
	mInitialized = false;
}
