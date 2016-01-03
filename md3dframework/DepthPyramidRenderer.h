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
	void Render(pTexture inSource, pRenderTarget inMaxTarget, pRenderTarget inMinTarget);

private:
	DepthPyramidRenderer(DepthPyramidRenderer const&) = delete;
	void operator=(DepthPyramidRenderer const&) = delete;

	bool		mInitialized = false;
};

