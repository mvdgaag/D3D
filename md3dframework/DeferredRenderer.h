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

REGISTERCLASS(DrawableObject);
REGISTERCLASS(GBuffer);
REGISTERCLASS(DeferredRenderer);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(Texture);
REGISTERCLASS(ConstantBuffer);


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
	DeferredRenderer(DeferredRenderer const&) = delete;
	void operator=(DeferredRenderer const&) = delete;

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
		float4x4 viewMatrix;
		float4x4 projectionMatrix;
		float4x4 viewProjectionMatrix;
		float4x4 inverseProjectionMatrix;
		float4 frameData;								// jitter_offset.xy, frameID, 0
	};

	struct ConstantDataEveryObject
	{
		float4x4 modelViewMatrix;
		float4x4 modelViewProjectionMatrix;
		float4x4 PrevModelViewProjectionMatrix;
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
	pConstantBuffer		mConstantBufferEveryObject = nullptr;
	float4x4			mPrevViewProjectionMatrix;

	bool mInitialized = false;
};

	