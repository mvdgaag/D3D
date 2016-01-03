#pragma once
#include "GaagCommon.h"
#include "float4.h"

// predeclarations
REGISTERCLASS(ComputeShader);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(ConstantBuffer);
REGISTERCLASS(GBuffer);
REGISTERCLASS(PointLight);
REGISTERCLASS(SpotLight);
REGISTERCLASS(DirectionalLight);
REGISTERCLASS(Camera);

#define MAX_POINT_LIGHTS 128
#define MAX_SPOT_LIGHTS 128
#define MAX_DIRECTIONAL_LIGHTS 1

class DirectLightingRenderer
{
public:
	DirectLightingRenderer() { mInitialized = false; }
	~DirectLightingRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pGBuffer inSource, pRenderTarget inTargetDiffuse, pRenderTarget inTargetSpecular,
		pRenderTarget inTargetDiffuseTemp, pRenderTarget inTargetSpecularTemp,
		apPointLight inPointLights, apSpotLight inSpotLights, apDirectionalLight inDirectionalLights);

private:
	DirectLightingRenderer(DirectLightingRenderer const&) = delete;
	void operator=(DirectLightingRenderer const&) = delete;

	struct ConstantBufferData
	{
		float4x4 projectionMatrix;
		float2 viewspaceReconstructionVector;
		float2 targetSize;
		float4 frameData;
	};

	struct ConstantBufferPointLightData
	{
		float4 lightPositions[MAX_POINT_LIGHTS];
		float4 lightColors[MAX_POINT_LIGHTS];
		float4 lightData;
	};

	struct ConstantBufferSpotLightData
	{
		float4 lightPositions[MAX_SPOT_LIGHTS];
		float4 lightDirections[MAX_SPOT_LIGHTS];
		float4 lightColors[MAX_SPOT_LIGHTS];
		float4 lightData;
	};

	struct ConstantBufferDirectionalLightData
	{
		float4 lightDirection;
		float4 lightColor;
		float4x4 viewToLightMatrix;
		float4 lightData;
	};

	ConstantBufferData mConstantBufferData;
	pConstantBuffer mConstantBuffer = nullptr;
	
	ConstantBufferPointLightData mConstantBufferPointLightData;
	pConstantBuffer mConstantBufferPointLights = nullptr;

	ConstantBufferSpotLightData mConstantBufferSpotLightData;
	pConstantBuffer mConstantBufferSpotLights = nullptr;

	ConstantBufferDirectionalLightData mConstantBufferDirectionalLightData;
	pConstantBuffer mConstantBufferDirectionalLights = nullptr;

	pComputeShader mDirectionalLightShader = nullptr;
	pComputeShader mPointLightShader = nullptr;
	pComputeShader mSpotLightShader = nullptr;
	bool mInitialized = false;
};


