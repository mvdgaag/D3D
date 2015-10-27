#pragma once
#include "GAAGCommon.h"

// predeclarations
PREDEFINE(ComputeShader, pComputeShader);
PREDEFINE(RenderTarget, pRenderTarget);
PREDEFINE(Texture, pTexture);


class LightComposeRenderer
{
public:
	LightComposeRenderer() { mInitialized = false; }
	~LightComposeRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pTexture inDirectDiffuse, pTexture inDirectSpecular, pTexture inIndirect, pTexture inReflections, pRenderTarget inTarget);

private:
	pComputeShader mShader = nullptr;
	bool mInitialized = false;
};