#pragma once
#include "GaagCommon.h"


// predeclarations
REGISTERCLASS(ComputeShader);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(Texture);
REGISTERCLASS(ConstantBuffer);


class IndirectLightingRenderer
{
public:
	IndirectLightingRenderer() { mInitialized = false; }
	~IndirectLightingRenderer() { CleanUp(); }
	
	void Init();
	void CleanUp();
	void Render(pTexture inSource, pTexture inNormal, pTexture inLinearDepth, pTexture inDiffuse, pRenderTarget inTarget, pRenderTarget inTempTarget);

private:
	IndirectLightingRenderer(IndirectLightingRenderer const&) = delete;
	void operator=(IndirectLightingRenderer const&) = delete;

	void ApplyIndirect(pTexture inSource, pTexture inNormal, pTexture inLinearDepth, pTexture inDiffuse, pRenderTarget inTarget);
	void ApplyBlur(pTexture inSource, pTexture inNormal, pTexture inLinearDepth, pRenderTarget inTarget, bool inHorizontal);

	struct ConstantBufferData
	{
		float2 viewspaceReconstructionVector;
		float2 targetSize;
		float4 frameData;
	};
	ConstantBufferData mConstantBufferData;
	pConstantBuffer mConstantBuffer = nullptr;
	pComputeShader mLightingShader = nullptr;
	pComputeShader mBlurShader = nullptr;
	bool mInitialized = false;
};