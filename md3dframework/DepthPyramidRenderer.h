#pragma once
#include "GaagCommon.h"

// predeclarations
REGISTERCLASS(ComputeShader);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(Texture);

class DepthPyramidRenderer
{
public:
	DepthPyramidRenderer() {};
	~DepthPyramidRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pTexture inSource, pRenderTarget inTarget);

private:
	DepthPyramidRenderer(DepthPyramidRenderer const&) = delete;
	void operator=(DepthPyramidRenderer const&) = delete;

	pComputeShader				mShader = nullptr;
	const int					kNumMipLevels = 3; // 1/2 .. 1/8
	bool						mInitialized = false;
};

