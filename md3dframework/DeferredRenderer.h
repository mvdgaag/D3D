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
class LightComposeRenderer;
class TAARenderer;
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
	RenderTarget* GetIndirectLighting() { return mIndirectLighting; }
	RenderTarget* GetReflections() { return mReflections; }
	RenderTarget* GetLightComposed() { return mLightComposed; }
	RenderTarget* GetAntiAliased() { return mAntiAliased; }
	RenderTarget* GetPostProcessed() { return mPostProcessed; }

private:
	void GeometryPass(std::vector<DrawableObject*> inDrawList);
	void LightingPass();
	void PostProcessPass();

	ShadowRenderer*				mShadowRenderer = nullptr;
	DirectLightingRenderer*		mDirectLightingRenderer = nullptr;
	DepthPyramidRenderer*		mDepthPyramidRenderer = nullptr;
	IndirectLightingRenderer*	mIndirectLightingRenderer = nullptr;
	ReflectionRenderer*			mReflectionRenderer = nullptr;
	LightComposeRenderer*		mLightComposeRenderer = nullptr;
	TAARenderer*				mTAARenderer = nullptr;
	PostProcessRenderer*		mPostProcessRenderer = nullptr;
	
	GBuffer* mGBuffer = nullptr;
	RenderTarget* mDepthPyramid = nullptr;
	RenderTarget* mDirectLighting = nullptr;
	RenderTarget* mIndirectLighting = nullptr;
	RenderTarget* mReflections = nullptr;
	RenderTarget* mLightComposed = nullptr;
	RenderTarget* mAntiAliased = nullptr;
	RenderTarget* mAAHistoryFrame = nullptr;
	RenderTarget* mPostProcessed = nullptr;

	ID3D11Device* mDevice = nullptr;

	bool mInitialized = false;
};

	