#include "LightComposeRenderer.h"
#include "ComputeShader.h"
#include "Framework.h"
#include "Texture.h"
#include "RenderTarget.h"

void LightComposeRenderer::Render(Texture* inDirect, Texture* inIndirect, Texture* inReflections, RenderTarget* inTarget)
{
	assert(mInitialized == true);

	assert(inDirect != nullptr);
	assert(inIndirect != nullptr);
	assert(inReflections != nullptr);
	assert(inTarget != nullptr);

	theFramework.SetComputeShader(mShader);
	theFramework.ComputeSetTextureAndSampler(inDirect, theFramework.GetPointSampler(), 0);
	theFramework.ComputeSetTextureAndSampler(inIndirect, theFramework.GetPointSampler(), 1);
	theFramework.ComputeSetTextureAndSampler(inReflections, theFramework.GetPointSampler(), 2);
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
}


void LightComposeRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader("LightComposeCompute");
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
