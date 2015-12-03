#include "DirectLightingRenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "GBuffer.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "PointLight.h"
#include <assert.h>


void DirectLightingRenderer::Render(pGBuffer inSource, pRenderTarget inTargetDiffuse, pRenderTarget inTargetSpecular, apPointLight inLights)
{
	assert(mInitialized == true);

	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();

	assert(inSource != nullptr);
	assert(inTargetDiffuse != nullptr);
	assert(inTargetSpecular != nullptr);
	assert(inTargetDiffuse->GetTexture()->GetWidth() == inTargetDiffuse->GetTexture()->GetWidth() &&
		inTargetDiffuse->GetTexture()->GetHeight() == inTargetDiffuse->GetTexture()->GetHeight());

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::LINEAR_DEPTH), point_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::NORMAL), point_sampler, 1);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::DIFFUSE), point_sampler, 2);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::MATERIAL), point_sampler, 3);
	theRenderContext.CSSetRWTexture(inTargetDiffuse, 0);
	theRenderContext.CSSetRWTexture(inTargetSpecular, 1);

	pCamera cam = Gaag.GetCamera();
	mConstantBufferData.projectionMatrix = transpose(cam->GetProjectionMatrix());
	mConstantBufferData.viewspaceReconstructionVector.x = tan(0.5f * cam->GetFovX());
	mConstantBufferData.viewspaceReconstructionVector.y = tan(0.5f * cam->GetFovY());
	mConstantBufferData.targetSize.x = (float)theRenderContext.GetWidth();
	mConstantBufferData.targetSize.y = (float)theRenderContext.GetHeight();
	mConstantBufferData.frameData.x = (float)Gaag.GetFrameID();

	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	for (int i = 0; i < inLights.size(); i++)
	{
		float4 pos = inLights[i]->GetPosition();
		float radius = pos.w;
		pos = float4(Gaag.WorldToCameraPos(float3(pos)), radius);
		mConstantBufferLightData.lightPositions[i] = pos;
		mConstantBufferLightData.lightColors[i] = inLights[i]->GetColor();
	}
	mConstantBufferLightData.lightData = float4(inLights.size(), 0, 0, 0);

	theRenderContext.UpdateSubResource(*mConstantBufferLight, &mConstantBufferLightData);
	theRenderContext.CSSetConstantBuffer(mConstantBufferLight, 1);

	int groups_x = (inTargetDiffuse->GetTexture()->GetWidth() + 7) / 8;
	int groups_y = (inTargetDiffuse->GetTexture()->GetHeight() + 7) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetConstantBuffer(NULL, 1);
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 3);
	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 1);
}


void DirectLightingRenderer::Init()
{
	CleanUp();
	mShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/DirectLightingCompute.hlsl");
	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferData));
	mConstantBufferLight = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferLightData));
	mInitialized = true;
}


void DirectLightingRenderer::CleanUp()
{
	mShader = nullptr;
	mConstantBuffer = nullptr;
	mConstantBufferLight = nullptr;
	mInitialized = false;
}
