#pragma once
#include "GaagCommon.h"


// predeclarations
REGISTERCLASS(ComputeShader);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(Texture);
REGISTERCLASS(ConstantBuffer);


class PostProcessRenderer
{
public:
	PostProcessRenderer() { mInitialized = false; }
	~PostProcessRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pTexture inSource, pTexture inMotionVectors, pRenderTarget inTarget);

private:
	PostProcessRenderer(PostProcessRenderer const&) = delete;
	void operator=(PostProcessRenderer const&) = delete;

	struct ConstantBufferData
	{
		float4 mTargetSize;
	};
	ConstantBufferData mConstantBufferData;

	pConstantBuffer mConstantBuffer = nullptr;
	pComputeShader mShader = nullptr;
	bool mInitialized = false;
};