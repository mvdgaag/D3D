#include "PostProcessRenderer.h"
#include "ComputeShader.h"


void PostProcessRenderer::Render()
{
}


void PostProcessRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader("PostProcessCompute");
	mShader->InitFromFile("Shaders/PostProcessCompute.hlsl");
}


void PostProcessRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
}


