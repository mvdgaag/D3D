#include "DeferredRenderer.h"
#include "GBuffer.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "ShadowRenderer.h"
#include "DirectLightingRenderer.h"
#include "DepthPyramidRenderer.h"
#include "IndirectLightingRenderer.h"
#include "ReflectionRenderer.h"
#include "PostProcessRenderer.h"
#include "DrawableObject.h"
#include <assert.h>


void DeferredRenderer::Init(int inWidth, int inHeight)
{
	assert(inWidth > 0 && inHeight > 0);

	CleanUp();

	mGBuffer = new GBuffer();
	mGBuffer->Init(inWidth, inHeight);
	
	mShadowRenderer = new ShadowRenderer();
	mShadowRenderer->Init();

	mDirectLightingRenderer = new DirectLightingRenderer();
	mDirectLightingRenderer->Init();

	mDepthPyramidRenderer = new DepthPyramidRenderer();
	mDepthPyramidRenderer->Init(mGBuffer->GetRenderTarget(GBuffer::LINEAR_DEPTH));

	mIndirectLightingRenderer = new IndirectLightingRenderer();
	mIndirectLightingRenderer->Init();

	mReflectionRenderer = new ReflectionRenderer();
	mReflectionRenderer->Init();

	mPostProcessRenderer = new PostProcessRenderer();
	mPostProcessRenderer->Init();

	mInitialized = true;	
}


void DeferredRenderer::CleanUp()
{
	delete mGBuffer;
	mGBuffer = NULL;
	delete mShadowRenderer;
	mShadowRenderer = NULL;
	delete mDirectLightingRenderer;
	mDirectLightingRenderer = NULL;
	delete mDepthPyramidRenderer;
	mDepthPyramidRenderer = NULL;
	delete mIndirectLightingRenderer;
	mIndirectLightingRenderer = NULL;
	delete mReflectionRenderer;
	mReflectionRenderer = NULL;
	delete mPostProcessRenderer;
	mPostProcessRenderer = NULL;
}


void DeferredRenderer::Render(ID3D11DeviceContext* inDeviceContext, std::vector<DrawableObject*> inDrawList)
{
	assert(mInitialized == true);
	GeometryPass(inDeviceContext, inDrawList);
	LightingPass(inDeviceContext);
}


void DeferredRenderer::GeometryPass(ID3D11DeviceContext* inDeviceContext, std::vector<DrawableObject*> inDrawList)
{
	for each (DrawableObject* obj in inDrawList)
		obj->Draw(inDeviceContext);
}


void DeferredRenderer::LightingPass(ID3D11DeviceContext* inDeviceContext)
{
	//mShadowRenderer->Render(inDeviceContext);
	//mDirectLightingRenderer->Render(inDeviceContext);
	//mDepthPyramidRenderer->Render(inDeviceContext);
	//mIndirectLightingRenderer->Render(inDeviceContext);
	//mReflectionRenderer->Render(inDeviceContext);
	// volumetrics and particles
	// exposure
	//mPostProcessRenderer->Render(inDeviceContext);
}