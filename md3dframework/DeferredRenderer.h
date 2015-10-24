#pragma once
#include <vector>
#include "LinearAlgebra.h"


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
class ConstantBuffer;


#define MAX_LIGHTS 64


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
	RenderTarget* GetDirectLightingDiffuse() { return mDirectLightingDiffuse; }
	RenderTarget* GetDirectLightingSpecular() { return mDirectLightingSpecular; }
	RenderTarget* GetIndirectLighting() { return mIndirectLighting; }
	RenderTarget* GetReflections() { return mReflections; }
	RenderTarget* GetLightComposed() { return mLightComposed; }
	RenderTarget* GetAntiAliased() { return mAntiAliased; }
	RenderTarget* GetPostProcessed() { return mPostProcessed; }

	ConstantBuffer* GetConstantBufferEveryFrame() { return mConstantBufferEveryFrame; }
	ConstantBuffer* GetConstantBufferOnDemand() { return mConstantBufferOnDemand; }

	// TODO:
	void AddLights() {}
	void RemoveLights() {}

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
	ConstantBuffer*				mConstantBufferOnDemand = nullptr;
	ConstantBuffer*				mConstantBufferEveryFrame = nullptr;
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
	
	GBuffer* mGBuffer = nullptr;
	RenderTarget* mDepthPyramid = nullptr;
	RenderTarget* mDirectLightingDiffuse = nullptr;
	RenderTarget* mDirectLightingSpecular = nullptr;
	RenderTarget* mIndirectLighting = nullptr;
	RenderTarget* mReflections = nullptr;
	RenderTarget* mLightComposed = nullptr;
	RenderTarget* mAntiAliased = nullptr;
	RenderTarget* mAAHistoryFrame = nullptr;
	RenderTarget* mPostProcessed = nullptr;

	bool mInitialized = false;
};

	