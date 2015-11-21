#pragma once
#include "GaagCommon.h"
#include "float4.h"


// predeclarations
PREDEFINE(ComputeShader, pComputeShader);
PREDEFINE(RenderTarget, pRenderTarget);
PREDEFINE(ConstantBuffer, pConstantBuffer);
PREDEFINE(GBuffer, pGBuffer);


class DirectLightingRenderer
{
public:
	DirectLightingRenderer() { mInitialized = false; }
	~DirectLightingRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pGBuffer inSource, pRenderTarget inTargetDiffuse, pRenderTarget inTargetSpecular);

private:
	DirectLightingRenderer(DirectLightingRenderer const&) = delete;
	void operator=(DirectLightingRenderer const&) = delete;

	struct ConstantBufferData
	{
		float2 viewspaceReconstructionVector;
		float2 targetSize;
		float4 frameData;
	};
	ConstantBufferData mConstantBufferData;
	pConstantBuffer mConstantBuffer = nullptr;
	pComputeShader mShader = nullptr;
	bool mInitialized = false;
};

