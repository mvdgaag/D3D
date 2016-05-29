#pragma once
#include "GaagCommon.h"


// predeclarations
REGISTERCLASS(ComputeShader);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(Texture);
REGISTERCLASS(ConstantBuffer);


class SkyLightRenderer
{
public:
	SkyLightRenderer() { mInitialized = false; }
	~SkyLightRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pTexture inNormal, pTexture inLinearDepth, pTexture inDiffuse, pRenderTarget inTarget);

private:
	SkyLightRenderer(SkyLightRenderer const&) = delete;
	void operator=(SkyLightRenderer const&) = delete;

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
