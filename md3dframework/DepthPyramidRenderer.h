#pragma once
#include <d3d11_1.h>


// predeclarations
class ComputeShader;
class Texture;
class RenderTarget;
class ShaderResourceBinding;
#include <d3d11_1.h>


class DepthPyramidRenderer
{
public:
	DepthPyramidRenderer() {};
	~DepthPyramidRenderer() { CleanUp(); }

	void Init(int inSourceWidth, int inSourceHeight);
	void CleanUp();
	void Render(Texture* inSource, RenderTarget* inTarget);

private:
	ComputeShader*				mShader = nullptr;
	const int					kNumMipLevels = 3; // 1/2 .. 1/8
	bool						mInitialized = false;
};

