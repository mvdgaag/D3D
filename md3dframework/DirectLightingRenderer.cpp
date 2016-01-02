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
#include "SpotLight.h"
#include "DirectionalLight.h"
#include <assert.h>


void DirectLightingRenderer::Render(pGBuffer inSource, pRenderTarget inTargetDiffuse, pRenderTarget inTargetSpecular, 
	apPointLight inPointLights, apSpotLight inSpotLights, apDirectionalLight inDirectionalLights)
{
	assert(mInitialized == true);

	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();

	assert(inSource != nullptr);
	assert(inTargetDiffuse != nullptr);
	assert(inTargetSpecular != nullptr);

	// set general constant buffer data
	pCamera cam = Gaag.GetCamera();
	mConstantBufferData.projectionMatrix = transpose(cam->GetProjectionMatrix());
	mConstantBufferData.viewspaceReconstructionVector.x = tan(0.5f * cam->GetFovX());
	mConstantBufferData.viewspaceReconstructionVector.y = tan(0.5f * cam->GetFovY());
	mConstantBufferData.targetSize.x = (float)theRenderContext.GetWidth();
	mConstantBufferData.targetSize.y = (float)theRenderContext.GetHeight();
	mConstantBufferData.frameData.x = (float)Gaag.GetFrameID();
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);

	// set constant buffer data for pointlights
	for (int i = 0; i < inPointLights.size(); i++)
	{
		float3 pos = inPointLights[i]->GetPosition();
		float radius = inPointLights[i]->GetRadius();
		mConstantBufferPointLightData.lightPositions[i] = float4(Gaag.WorldToCameraPos(float3(pos)), radius);
		mConstantBufferPointLightData.lightColors[i] = inPointLights[i]->GetColor();
	}
	mConstantBufferPointLightData.lightData = float4(inPointLights.size(), 0, 0, 0);
	theRenderContext.UpdateSubResource(*mConstantBufferPointLights, &mConstantBufferPointLightData);

	// set constant buffer data for spotligths
	for (int i = 0; i < inSpotLights.size(); i++)
	{
		float3 pos = inSpotLights[i]->GetPosition();
		float radius = inSpotLights[i]->GetRadius();
		mConstantBufferSpotLightData.lightPositions[i] = float4(Gaag.WorldToCameraPos(float3(pos)), radius);

		float3 dir = Gaag.WorldToCameraNormal(inSpotLights[i]->GetDirection());
		float cone_cos = inSpotLights[i]->GetConeCosine();
		mConstantBufferSpotLightData.lightDirections[i] = float4(dir, cone_cos);

		mConstantBufferSpotLightData.lightColors[i] = inSpotLights[i]->GetColor();
	}
	mConstantBufferSpotLightData.lightData = float4(inSpotLights.size(), 0, 0, 0);
	theRenderContext.UpdateSubResource(*mConstantBufferSpotLights, &mConstantBufferSpotLightData);
	
	// set constant buffer data for directional ligths
	for (int i = 0; i < inDirectionalLights.size(); i++)
	{
		mConstantBufferDirectionalLightData.lightDirections[i] = float4(Gaag.WorldToCameraNormal(float3(-inDirectionalLights[i]->GetDirection())), 1.0);
		float4 color = inDirectionalLights[i]->GetColor();
		color.w = (inDirectionalLights[i]->GetShadowMap() == nullptr) ? 0.0 : 1.0;
		mConstantBufferDirectionalLightData.lightColors[i] = color;
		float4x4& shadow_matrix = inDirectionalLights[i]->GetShadowMatrix();
		float4x4 inverse_view = inverse(cam->GetViewMatrix());
		float4x4 view_to_light = shadow_matrix * inverse_view;
		mConstantBufferDirectionalLightData.invShadowMatrices[i] = transpose(view_to_light);
	}
	mConstantBufferDirectionalLightData.lightData = float4(inDirectionalLights.size(), 0, 0, 0);
	theRenderContext.UpdateSubResource(*mConstantBufferDirectionalLights, &mConstantBufferDirectionalLightData);

	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::LINEAR_DEPTH), point_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::NORMAL), point_sampler, 1);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::DIFFUSE), point_sampler, 2);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::MATERIAL), point_sampler, 3);
	for (int i = 0; i < inDirectionalLights.size(); i++)
	{
		pRenderTarget shadow_map = inDirectionalLights[i]->GetShadowMap();
		if (shadow_map != nullptr)
			theRenderContext.CSSetTextureAndSampler(shadow_map->GetTexture(), point_sampler, i + 4);
	}
	theRenderContext.CSSetRWTexture(inTargetDiffuse, 0);
	theRenderContext.CSSetRWTexture(inTargetSpecular, 1);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	// dispatch
	if (inPointLights.size() > 0 || inSpotLights.size() > 0 || inDirectionalLights.size() > 0)
	{
		theRenderContext.CSSetShader(mShader);
		theRenderContext.CSSetConstantBuffer(mConstantBufferPointLights, 1);
		theRenderContext.CSSetConstantBuffer(mConstantBufferSpotLights, 2);
		theRenderContext.CSSetConstantBuffer(mConstantBufferDirectionalLights, 3);

		assert(inTargetDiffuse->GetDimensions() == inTargetSpecular->GetDimensions());
		int2 groups = (inTargetDiffuse->GetDimensions() + 7) / 8;
		theRenderContext.Dispatch(groups.x, groups.y, 1);
		theRenderContext.Flush();
	}

	// clear state
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetConstantBuffer(NULL, 1);
	theRenderContext.CSSetConstantBuffer(NULL, 2);
	theRenderContext.CSSetConstantBuffer(NULL, 3);
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 3);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 4);
	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 1);
}


void DirectLightingRenderer::Init()
{
	assert(mInitialized == false);
	mShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/DirectLightingCompute.hlsl");
	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferData));
	mConstantBufferPointLights = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferPointLightData));
	mConstantBufferSpotLights = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferSpotLightData));
	mConstantBufferDirectionalLights = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferDirectionalLightData));
	mConstantBufferPointLightData.lightData = float4(0.0f);
	mConstantBufferSpotLightData.lightData = float4(0.0f);
	mConstantBufferDirectionalLightData.lightData = float4(0.0f);
	mInitialized = true;
}


void DirectLightingRenderer::CleanUp()
{
	theResourceFactory.DestroyItem(mShader);
	theResourceFactory.DestroyItem(mConstantBuffer);
	theResourceFactory.DestroyItem(mConstantBufferPointLights);
	theResourceFactory.DestroyItem(mConstantBufferSpotLights);
	theResourceFactory.DestroyItem(mConstantBufferDirectionalLights);

	mShader = nullptr;
	mConstantBuffer = nullptr;
	mConstantBufferPointLights = nullptr;
	mConstantBufferSpotLights = nullptr;
	mConstantBufferDirectionalLights = nullptr;
	mInitialized = false;
}
