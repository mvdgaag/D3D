#pragma once
#include "GaagCommon.h"

// predeclarations
PREDEFINE(ConstantBuffer, pConstantBuffer);
PREDEFINE(ComputeShader, pComputeShader);
PREDEFINE(RenderTarget, pRenderTarget);
PREDEFINE(Texture, pTexture);


class TAARenderer
{
public:
	TAARenderer() { mInitialized = false; }
	~TAARenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pTexture inSource, pRenderTarget inHistory, pTexture inMotionVectors, pRenderTarget inTarget);
	
	static float2 GetJitterOffset(int inFrameID);

private:
	TAARenderer(TAARenderer const&) = delete;
	void operator=(TAARenderer const&) = delete;

	struct ConstantBufferData
	{
		float2 mJitterOffset;
		float2 mTargetSize;
	};
	ConstantBufferData mConstantBufferData;
	pConstantBuffer mConstantBuffer;
	pComputeShader mShader = nullptr;
	bool mInitialized = false;
};