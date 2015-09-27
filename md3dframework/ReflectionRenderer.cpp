#include "ReflectionRenderer.h"
#include "ComputeShader.h"


void ReflectionRenderer::Render(ID3D11DeviceContext* inDeviceContext)
{
}


void ReflectionRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader("ReflectionRendererCompute");
	mShader->InitFromFile("Shaders/ReflectionCompute.hlsl");
}


void ReflectionRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
}

