#include "DirectLightingRenderer.h"
#include "ComputeShader.h"
#include "GBuffer.h"
#include "RenderTarget.h"


void DirectLightingRenderer::Render(GBuffer* inSource, RenderTarget* inTarget)
{
}


void DirectLightingRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader("DirectLightingCompute");
	mShader->InitFromFile("Shaders/DirectLightingCompute.hlsl");
}


void DirectLightingRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
}