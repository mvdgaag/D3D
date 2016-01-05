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
	pRenderTarget inTargetDiffuseTemp, pRenderTarget inTargetSpecularTemp,
	apPointLight inPointLights, apSpotLight inSpotLights, apDirectionalLight inDirectionalLights)
{
	assert(mInitialized == true);
	assert(inSource != nullptr);
	assert(inTargetDiffuse != nullptr);
	assert(inTargetSpecular != nullptr);
	assert(inTargetDiffuse->GetDimensions() == inTargetSpecular->GetDimensions());
	assert(inDirectionalLights.size() <= MAX_DIRECTIONAL_LIGHTS);
	assert(inPointLights.size() <= MAX_POINT_LIGHTS);
	assert(inSpotLights.size() <= MAX_SPOT_LIGHTS);

	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();
	int2 groups = (inTargetDiffuse->GetDimensions() + 7) / 8;

	// set general constant buffer data
	pCamera cam = Gaag.GetCamera();
	mConstantBufferData.projectionMatrix = transpose(cam->GetProjectionMatrix());
	mConstantBufferData.viewspaceReconstructionVector.x = 1.0 / cam->GetProjectionMatrix()[0][0];
	mConstantBufferData.viewspaceReconstructionVector.y = 1.0 / cam->GetProjectionMatrix()[1][1];
	mConstantBufferData.targetSize.x = (float)theRenderContext.GetWidth();
	mConstantBufferData.targetSize.y = (float)theRenderContext.GetHeight();
	mConstantBufferData.frameData.x = (float)Gaag.GetFrameID();
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);

	theRenderContext.CSSetRWTexture(inTargetDiffuse, 0, 0);
	theRenderContext.CSSetRWTexture(inTargetSpecular, 0, 1);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::LINEAR_DEPTH), point_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::NORMAL), point_sampler, 1);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::DIFFUSE), point_sampler, 2);
	theRenderContext.CSSetTextureAndSampler(inSource->GetTexture(GBuffer::MATERIAL), point_sampler, 3);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	// render directional lights
	theRenderContext.CSSetShader(mDirectionalLightShader);
	mConstantBufferDirectionalLightData.lightData = float4(inDirectionalLights.size(), 0, 0, 0);
	for (int i = 0; i < inDirectionalLights.size(); i++)
	{
		// set constant data for directional light
		mConstantBufferDirectionalLightData.lightDirection = float4(Gaag.WorldToCameraNormal(float3(-inDirectionalLights[i]->GetDirection())), 1.0);
		float4 color = inDirectionalLights[i]->GetColor();
		color.w = (inDirectionalLights[i]->GetShadowMap() == nullptr) ? 0.0 : 1.0;
		mConstantBufferDirectionalLightData.lightColor = color;
		float4x4& shadow_matrix = inDirectionalLights[i]->GetShadowMatrix();
		float4x4 inverse_view = inverse(cam->GetViewMatrix());
		float4x4 view_to_light = shadow_matrix * inverse_view;
		mConstantBufferDirectionalLightData.viewToLightMatrix = transpose(view_to_light);

		theRenderContext.UpdateSubResource(*mConstantBufferDirectionalLights, &mConstantBufferDirectionalLightData);
		theRenderContext.CSSetConstantBuffer(mConstantBufferDirectionalLights, 1);

		pRenderTarget shadow_map = inDirectionalLights[i]->GetShadowMap();
		if (shadow_map != nullptr)
			theRenderContext.CSSetTextureAndSampler(shadow_map->GetTexture(), point_sampler, 4);

		theRenderContext.Dispatch(groups.x, groups.y, 1);
		theRenderContext.Flush();
	}

	// render point lights
	theRenderContext.CSSetShader(mPointLightShader);

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
	theRenderContext.CSSetConstantBuffer(mConstantBufferPointLights, 1);

	theRenderContext.CSSetRWTexture(inTargetDiffuseTemp, 0, 0);
	theRenderContext.CSSetRWTexture(inTargetSpecularTemp, 0, 1);
	theRenderContext.CSSetTextureAndSampler(inTargetDiffuse->GetTexture(), point_sampler, 4);
	theRenderContext.CSSetTextureAndSampler(inTargetSpecular->GetTexture(), point_sampler, 5);

	theRenderContext.Dispatch(groups.x, groups.y, 1);
	theRenderContext.Flush();

	// render spotlights
	theRenderContext.CSSetShader(mSpotLightShader);

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
	theRenderContext.CSSetConstantBuffer(mConstantBufferSpotLights, 1);

	theRenderContext.CSSetRWTexture(inTargetDiffuse, 0, 0);
	theRenderContext.CSSetRWTexture(inTargetSpecular, 0, 1);
	theRenderContext.CSSetTextureAndSampler(inTargetDiffuseTemp->GetTexture(), point_sampler, 4);
	theRenderContext.CSSetTextureAndSampler(inTargetSpecularTemp->GetTexture(), point_sampler, 5);

	theRenderContext.Dispatch(groups.x, groups.y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetConstantBuffer(NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 3);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 4);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 5);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 6);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
	theRenderContext.CSSetRWTexture(NULL, 0, 1);
}



void DirectLightingRenderer::Init()
{
	assert(mInitialized == false);
	mDirectionalLightShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/DirectionalLightCompute.hlsl");
	mPointLightShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/PointLightCompute.hlsl");
	mSpotLightShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/SpotLightCompute.hlsl");
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
	theResourceFactory.DestroyItem(mDirectionalLightShader);
	theResourceFactory.DestroyItem(mPointLightShader);
	theResourceFactory.DestroyItem(mSpotLightShader);
	theResourceFactory.DestroyItem(mConstantBuffer);
	theResourceFactory.DestroyItem(mConstantBufferPointLights);
	theResourceFactory.DestroyItem(mConstantBufferSpotLights);
	theResourceFactory.DestroyItem(mConstantBufferDirectionalLights);

	mDirectionalLightShader = nullptr;
	mPointLightShader = nullptr;
	mSpotLightShader = nullptr;
	mConstantBuffer = nullptr;
	mConstantBufferPointLights = nullptr;
	mConstantBufferSpotLights = nullptr;
	mConstantBufferDirectionalLights = nullptr;
	mInitialized = false;
}
