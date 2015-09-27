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
	DepthPyramidRenderer();
	~DepthPyramidRenderer() { CleanUp(); }

	void Init(RenderTarget* inSource);
	void CleanUp();
	void Render(ID3D11DeviceContext* inDeviceContext);
	Texture* GetDepthPyramidTexture() { return mTargetTexture; }

private:
	ComputeShader*		mShader = nullptr;

	Texture*			mSourceTexture = nullptr;
	Texture*			mTargetTexture = nullptr;

	RenderTarget*		mRenderTargets[5];
	const int			kNumMipLevels = 5;

	bool mInitialized = false;
};

