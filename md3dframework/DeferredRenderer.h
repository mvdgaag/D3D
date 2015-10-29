#pragma once
#include <vector>
#include "GaagCommon.h"
#include "ShadowRenderer.h"
#include "DirectLightingRenderer.h"
#include "DepthPyramidRenderer.h"
#include "IndirectLightingRenderer.h"
#include "ReflectionRenderer.h"
#include "PostProcessRenderer.h"
#include "LightComposeRenderer.h"
#include "TAARenderer.h"
#include "PostProcessRenderer.h"

PREDEFINE(DrawableObject, pDrawableObject);
PREDEFINE(GBuffer, pGBuffer);
PREDEFINE(DeferredRenderer, pDeferredRenderer);
PREDEFINE(RenderTarget, pRenderTarget);
PREDEFINE(Texture, pTexture);
PREDEFINE(ConstantBuffer, pConstantBuffer);


#define MAX_LIGHTS 64


class DeferredRenderer
{
public:
	DeferredRenderer() : mInitialized(false) {}
	~DeferredRenderer() { CleanUp(); }

	void Init(int inWidth, int inHeight);
	void CleanUp();
	void Render(std::vector<pDrawableObject> inDrawList);

	pGBuffer GetGBuffer() { return mGBuffer; }
	pRenderTarget GetDepthPyramid() { return mDepthPyramid; }
	pRenderTarget GetDirectLightingDiffuse() { return mDirectLightingDiffuse; }
	pRenderTarget GetDirectLightingSpecular() { return mDirectLightingSpecular; }
	pRenderTarget GetIndirectLighting() { return mIndirectLighting; }
	pRenderTarget GetReflections() { return mReflections; }
	pRenderTarget GetLightComposed() { return mLightComposed; }
	pRenderTarget GetAntiAliased() { return mAntiAliased; }
	pRenderTarget GetPostProcessed() { return mPostProcessed; }

	pConstantBuffer GetConstantBufferEveryFrame() { return mConstantBufferEveryFrame; }
	pConstantBuffer GetConstantBufferOnDemand() { return mConstantBufferOnDemand; }

	// TODO:
	void AddLights() {}
	void RemoveLights() {}

private:
	void GeometryPass(std::vector<pDrawableObject> inDrawList);
	void LightingPass();
	void PostProcessPass();

	ShadowRenderer				mShadowRenderer;
	DirectLightingRenderer		mDirectLightingRenderer;
	DepthPyramidRenderer		mDepthPyramidRenderer;
	IndirectLightingRenderer	mIndirectLightingRenderer;
	ReflectionRenderer			mReflectionRenderer;
	LightComposeRenderer		mLightComposeRenderer;
	TAARenderer					mTAARenderer;
	PostProcessRenderer			mPostProcessRenderer;
	
	struct ConstantDataEveryFrame
	{
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;
		DirectX::XMMATRIX viewProjectionMatrix;
		DirectX::XMMATRIX inverseProjectionMatrix;
		float4 frameData;								// jitter_offset.xy, frameID, 0
	};
	struct ConstantDataOnDemand
	{
		float4 lightPositions[MAX_LIGHTS];
		float4 lightColors[MAX_LIGHTS];
	};
	
	pGBuffer			mGBuffer = nullptr;
	pRenderTarget		mDepthPyramid = nullptr;
	pRenderTarget		mDirectLightingDiffuse = nullptr;
	pRenderTarget		mDirectLightingSpecular = nullptr;
	pRenderTarget		mIndirectLighting = nullptr;
	pRenderTarget		mReflections = nullptr;
	pRenderTarget		mLightComposed = nullptr;
	pRenderTarget		mAntiAliased = nullptr;
	pRenderTarget		mAAHistoryFrame = nullptr;
	pRenderTarget		mPostProcessed = nullptr;
	pConstantBuffer		mConstantBufferOnDemand = nullptr;
	pConstantBuffer		mConstantBufferEveryFrame = nullptr;

	bool mInitialized = false;
};

	