#pragma once
#include "GaagCommon.h"
#include "float4.h"

// predeclarations
REGISTERCLASS(ComputeShader);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(ConstantBuffer);
REGISTERCLASS(GBuffer);
REGISTERCLASS(PointLight);
REGISTERCLASS(Camera);

#define MAX_LIGHTS 256


class DirectLightingRenderer
{
public:
	DirectLightingRenderer() { mInitialized = false; }
	~DirectLightingRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pGBuffer inSource, pRenderTarget inTargetDiffuse, pRenderTarget inTargetSpecular, apPointLight inLights);

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

	struct ConstantBufferLightData
	{
		float4 lightPositions[MAX_LIGHTS];
		float4 lightColors[MAX_LIGHTS];
		float4 lightData;
	};

	struct 
	ConstantBufferData mConstantBufferData;
	pConstantBuffer mConstantBuffer = nullptr;
	ConstantBufferLightData mConstantBufferLightData;
	pConstantBuffer mConstantBufferLight = nullptr;
	pComputeShader mShader = nullptr;
	bool mInitialized = false;
};

