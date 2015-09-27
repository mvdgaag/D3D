#include "IndirectLightingRenderer.h"
#include "ComputeShader.h"


void IndirectLightingRenderer::Render(ID3D11DeviceContext* inDeviceContext)
{
}


void IndirectLightingRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader("IndirectLightingCompute");
	mShader->InitFromFile("Shaders/IndirectLightingCompute.hlsl");
}


void IndirectLightingRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
}
