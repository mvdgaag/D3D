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
REGISTERCLASS(Camera);

#define MAX_POINT_LIGHTS 256 // needs to match DirectLightingCompute.hlsl
#define MAX_SPOT_LIGHTS 256 // needs to match DirectLightingCompute.hlsl


class DirectLightingRenderer
{
public:
	DirectLightingRenderer() { mInitialized = false; }
	~DirectLightingRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pGBuffer inSource, pRenderTarget inTargetDiffuse, pRenderTarget inTargetSpecular, apPointLight inPointLights, apSpotLight inSpotLights);

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

	ConstantBufferData mConstantBufferData;
	pConstantBuffer mConstantBuffer = nullptr;
	
	ConstantBufferPointLightData mConstantBufferPointLightData;
	pConstantBuffer mConstantBufferPointLights = nullptr;

	ConstantBufferSpotLightData mConstantBufferSpotLightData;
	pConstantBuffer mConstantBufferSpotLights = nullptr;

	pComputeShader mShader = nullptr;
	bool mInitialized = false;
};


