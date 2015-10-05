#include "TAARenderer.h"
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

	theFramework.SetComputeShader(mShader);
	theFramework.ComputeSetTextureAndSampler(inSource, theFramework.GetPointSampler(), 0);
	theFramework.ComputeSetTextureAndSampler(inHistory->GetTexture(), theFramework.GetLinearSampler(), 1);
	theFramework.ComputeSetTextureAndSampler(inMotionVectors, theFramework.GetPointSampler(), 2);
	theFramework.ComputeSetRWTexture(inTarget, 0);

	int groups_x = 1 + (inTarget->GetTexture()->GetWidth() - 1) / 8;
	int groups_y = 1 + (inTarget->GetTexture()->GetHeight() - 1) / 8;
	theFramework.ComputeDispatch(groups_x, groups_y, 1);

	// TODO: required?
	theFramework.Flush();

	// clear state
	theFramework.SetComputeShader(NULL);
	theFramework.ComputeSetTextureAndSampler(NULL, NULL, 0);
	theFramework.ComputeSetTextureAndSampler(NULL, NULL, 1);
	theFramework.ComputeSetTextureAndSampler(NULL, NULL, 2);
	theFramework.ComputeSetRWTexture(NULL, 0);

	// make history copy;
	theFramework.CopyToRenderTarget(inHistory, inTarget->GetTexture());
}


void TAARenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader("TemporalAACompute");
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
