#include "LightComposeRenderer.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "RenderContext.h"
#include "Texture.h"
#include "RenderTarget.h"


void LightComposeRenderer::Render(pTexture inDirectDiffuse, pTexture inDirectSpecular, pTexture inIndirect, pTexture inReflections, pRenderTarget inTarget)
{
	assert(mInitialized == true);

	assert(inDirectDiffuse != nullptr);
	assert(inDirectSpecular != nullptr);
	assert(inIndirect != nullptr);
	assert(inReflections != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inDirectDiffuse, Gaag.GetPointSampler(), 0);
	theRenderContext.CSSetTextureAndSampler(inDirectSpecular, Gaag.GetPointSampler(), 1);
	theRenderContext.CSSetTextureAndSampler(inIndirect, Gaag.GetPointSampler(), 2);
	theRenderContext.CSSetTextureAndSampler(inReflections, Gaag.GetPointSampler(), 3);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	int groups_x = (inTarget->GetTexture()->GetWidth() + 7) / 8;
	int groups_y = (inTarget->GetTexture()->GetHeight() + 7) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);
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
	mShader = std::make_shared<ComputeShader>();
	mShader->InitFromFile("../md3dFramework/Shaders/LightComposeCompute.hlsl");
	mInitialized = true;
}


void LightComposeRenderer::CleanUp()
{
	mShader = nullptr;
	mInitialized = false;
}
