#include "DepthPyramidRenderer.h"
#include "RenderContext.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "LinearAlgebra.h"
#include <assert.h>


void DepthPyramidRenderer::Render(Texture* inSource, RenderTarget* inTarget)
{
	assert(mInitialized);
	assert(inSource != nullptr);
	assert(inTarget != nullptr);
	assert(inTarget->GetTexture()->GetMipLevels() == kNumMipLevels);

	// We now set up the shader and run it
	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTexture(inSource, 0);

	theRenderContext.CSSetRWTexture(inTarget, 0);
	theRenderContext.CSSetRWTexture(inTarget, 1); // TODO mip
	theRenderContext.CSSetRWTexture(inTarget, 2); // TODO mip

	int groups_x = (inTarget->GetTexture()->GetWidth() + 7) / 8;
	int groups_y = (inTarget->GetTexture()->GetHeight() + 7) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);
	
	// TODO: required?
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 1);
	theRenderContext.CSSetRWTexture(NULL, 2);
	theRenderContext.CSSetShader(NULL);
}


void DepthPyramidRenderer::Init(int inSourceWidth, int inSourceHeight)
{
	mShader = new ComputeShader();
	mShader->InitFromFile("Shaders/DepthPyramidCompute.hlsl");
	assert(mShader != nullptr);
	mInitialized = true;
}


void DepthPyramidRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = nullptr;
	}
	mInitialized = false;
}

