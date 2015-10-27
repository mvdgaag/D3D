#pragma once
#include "GAAGCommon.h"

// predeclarations
PREDEFINE(ComputeShader, pComputeShader);
PREDEFINE(RenderTarget, pRenderTarget);
PREDEFINE(Texture, pTexture);

class DepthPyramidRenderer
{
public:
	DepthPyramidRenderer() {};
	~DepthPyramidRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pTexture inSource, pRenderTarget inTarget);

private:
	pComputeShader				mShader = nullptr;
	const int					kNumMipLevels = 3; // 1/2 .. 1/8
	bool						mInitialized = false;
};

