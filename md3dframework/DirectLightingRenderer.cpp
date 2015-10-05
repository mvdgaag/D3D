#include "DirectLightingRenderer.h"
#include "Framework.h"
#include "ComputeShader.h"
#include "GBuffer.h"
#include "RenderTarget.h"
#include "Texture.h"
#include <assert.h>


void DirectLightingRenderer::Render(GBuffer* inSource, RenderTarget* inTarget)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theFramework.SetComputeShader(mShader);
	theFramework.ComputeSetTextureAndSampler(inSource->GetTexture(GBuffer::LINEAR_DEPTH), theFramework.GetPointSampler(), 0);
	theFramework.ComputeSetTextureAndSampler(inSource->GetTexture(GBuffer::NORMAL), theFramework.GetPointSampler(), 1);
	theFramework.ComputeSetTextureAndSampler(inSource->GetTexture(GBuffer::DIFFUSE), theFramework.GetPointSampler(), 2);
	theFramework.ComputeSetTextureAndSampler(inSource->GetTexture(GBuffer::MATERIAL), theFramework.GetPointSampler(), 3);
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
	theFramework.ComputeSetTextureAndSampler(NULL, NULL, 3);
	theFramework.ComputeSetRWTexture(NULL, 0);
}


void DirectLightingRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader("DirectLightingCompute");
	mShader->InitFromFile("Shaders/DirectLightingCompute.hlsl");
	mInitialized = true;
}


void DirectLightingRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
	mInitialized = false;
}