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


class DeferredRenderer
{
public:
	DeferredRenderer() : mInitialized(false) {}
	~DeferredRenderer() { CleanUp(); }

	void Init(int inWidth, int inHeight);
	void CleanUp();
	void Render(ID3D11DeviceContext* inDeviceContext, std::vector<DrawableObject*> inDrawList);
	GBuffer* GetGBuffer() { return mGBuffer; }

private:
	void GeometryPass(ID3D11DeviceContext* inDeviceContext, std::vector<DrawableObject*> inDrawList);
	void LightingPass(ID3D11DeviceContext* inDeviceContext);

	ShadowRenderer*				mShadowRenderer = nullptr;
	DirectLightingRenderer*		mDirectLightingRenderer = nullptr;
	DepthPyramidRenderer*		mDepthPyramidRenderer = nullptr;
	IndirectLightingRenderer*	mIndirectLightingRenderer = nullptr;
	ReflectionRenderer*			mReflectionRenderer = nullptr;
	PostProcessRenderer*		mPostProcessRenderer = nullptr;
	
	GBuffer* mGBuffer = nullptr;

	ID3D11Device* mDevice = nullptr;

	bool mInitialized = false;
};

