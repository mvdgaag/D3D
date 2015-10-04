#pragma once
#include <d3d11_1.h>
#include <vector>


// predefinitions
class GBuffer;
class ShadowRenderer;
class DirectLightingRenderer;
class DepthPyramidRenderer;
class IndirectLightingRenderer;
class ReflectionRenderer;
class PostProcessRenderer;
class DrawableObject;
class RenderTarget;
class Texture;

class DeferredRenderer
{
public:
	DeferredRenderer() : mInitialized(false) {}
	~DeferredRenderer() { CleanUp(); }

	void Init(int inWidth, int inHeight);
	void CleanUp();
	void Render(std::vector<DrawableObject*> inDrawList);
	GBuffer* GetGBuffer() { return mGBuffer; }
	RenderTarget* GetDepthPyramid() { return mDepthPyramid; }
	RenderTarget* GetDirectLighting() { return mDirectLighting; }

private:
	void GeometryPass(std::vector<DrawableObject*> inDrawList);
	void LightingPass();

	ShadowRenderer*				mShadowRenderer = nullptr;
	DirectLightingRenderer*		mDirectLightingRenderer = nullptr;
	DepthPyramidRenderer*		mDepthPyramidRenderer = nullptr;
	IndirectLightingRenderer*	mIndirectLightingRenderer = nullptr;
	ReflectionRenderer*			mReflectionRenderer = nullptr;
	PostProcessRenderer*		mPostProcessRenderer = nullptr;
	
	GBuffer* mGBuffer = nullptr;
	RenderTarget* mDepthPyramid = nullptr;
	RenderTarget* mDirectLighting = nullptr;

	ID3D11Device* mDevice = nullptr;

	bool mInitialized = false;
};

