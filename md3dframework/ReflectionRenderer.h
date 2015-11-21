#pragma once
#include "GaagCommon.h"

// predeclarations
PREDEFINE(ComputeShader, pComputeShader);
PREDEFINE(RenderTarget, pRenderTarget);
PREDEFINE(Texture, pTexture);


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

