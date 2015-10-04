#pragma once
#include <d3d11_1.h>


// predeclarations
class ComputeShader;
class RenderTarget;
class Texture;


class TAARenderer
{
public:
	TAARenderer() { mInitialized = false; }
	~TAARenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(Texture* inSource, RenderTarget* inTarget);

private:
	ComputeShader* mShader = nullptr;
	bool mInitialized = false;
};