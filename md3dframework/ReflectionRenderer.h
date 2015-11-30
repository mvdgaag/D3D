#pragma once
#include "GaagCommon.h"

// predeclarations
REGISTERCLASS(ComputeShader);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(Texture);


class ReflectionRenderer
{
public:
	ReflectionRenderer() { mInitialized = false; }
	~ReflectionRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pTexture inSource, pRenderTarget inTarget);

private:
	ReflectionRenderer(ReflectionRenderer const&) = delete;
	void operator=(ReflectionRenderer const&) = delete;

	pComputeShader mShader = nullptr;
	bool mInitialized = false;
};

