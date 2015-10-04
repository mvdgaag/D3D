#pragma once
#include <d3d11_1.h>


// predeclarations
class ComputeShader;
class RenderTarget;
class Texture;


class LightComposeRenderer
{
public:
	LightComposeRenderer() { mInitialized = false; }
	~LightComposeRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(Texture* inDirect, Texture* inIndirect, Texture* inReflections, RenderTarget* inTarget);

private:
	ComputeShader* mShader = nullptr;
	bool mInitialized = false;
};