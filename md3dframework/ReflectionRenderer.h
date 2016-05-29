#pragma once
#include "GaagCommon.h"

// predeclarations
REGISTERCLASS(ComputeShader);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(Texture);
REGISTERCLASS(ConstantBuffer);


class ReflectionRenderer
{
public:
	ReflectionRenderer() { mInitialized = false; }
	~ReflectionRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pTexture inSource, pRenderTarget inTarget, pTexture inNormal, pTexture inMaterial, pTexture inDepth, pTexture inCubemap);
	void FilterCubemap(pRenderTarget inOutCubemap) const;

private:
	ReflectionRenderer(ReflectionRenderer const&) = delete;
	void operator=(ReflectionRenderer const&) = delete;	
	void GenerateBRDFLookup(pRenderTarget outLookup) const;

	struct ConstantBufferData
	{
		float2 viewspaceReconstructionVector;
		float2 targetSize;
		float4 params;
	};

	struct CubemapFilterConstantBufferData
	{
		float4 params;
	};

	struct BRDFLookupConstantBufferData
	{
		float4 params;
	};

	ConstantBufferData mConstantBufferData;
	pConstantBuffer mConstantBuffer = nullptr;
	pConstantBuffer mCubemapFilterConstantBuffer = nullptr;
	pConstantBuffer mGenerateBRDFLookupConstantBuffer = nullptr;

	pComputeShader mShader = nullptr;
	pComputeShader mCubemapFilterShader = nullptr;
	pComputeShader mGenerateBRDFLookupShader = nullptr;
	pRenderTarget mBRDFLookupTarget = nullptr;
	
	bool mInitialized = false;
};

