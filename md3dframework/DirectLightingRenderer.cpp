#include "DirectLightingRenderer.h"
#include "ComputeShader.h"


void DirectLightingRenderer::Render(ID3D11DeviceContext* inDeviceContext)
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