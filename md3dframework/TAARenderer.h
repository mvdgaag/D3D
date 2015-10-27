#pragma once
#include "GAAGCommon.h"

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