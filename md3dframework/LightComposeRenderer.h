#pragma once
#include "GaagCommon.h"

// predeclarations
REGISTERCLASS(ComputeShader);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(Texture);


class LightComposeRenderer
{
public:
	LightComposeRenderer() { mInitialized = false; }
	~LightComposeRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pTexture inDirectDiffuse, pTexture inDirectSpecular, pTexture inIndirect, pTexture inReflections, pRenderTarget inTarget);

private:
	LightComposeRenderer(LightComposeRenderer const&) = delete;
	void operator=(LightComposeRenderer const&) = delete;

	pComputeShader mShader = nullptr;
	bool mInitialized = false;
};