#pragma once
#include <d3d11_1.h>


// predeclarations
class ComputeShader;
class RenderTarget;
class GBuffer;


class DirectLightingRenderer
{
public:
	DirectLightingRenderer() {}
	~DirectLightingRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(GBuffer* inSource, RenderTarget* inTarget);

private:
	ComputeShader* mShader = nullptr;
};

