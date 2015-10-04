#include "DeferredRenderer.h"
#include "FrameWork.h"
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
#include <directxcolors.h>


void DeferredRenderer::Init(int inWidth, int inHeight)
{
	assert(inWidth > 0 && inHeight > 0);

	CleanUp();

	mGBuffer = new GBuffer();
	mGBuffer->Init(inWidth, inHeight);

	mDepthPyramid = new RenderTarget("DepthPyramidRenderTarget");
	mDepthPyramid->Init(inWidth / 2, inHeight / 2, 3, DXGI_FORMAT_R16G16_FLOAT);
	
	mDirectLighting = new RenderTarget("DirectLightingRenderTarget");
	mDirectLighting->Init(inWidth, inHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mShadowRenderer = new ShadowRenderer();
	mShadowRenderer->Init();

	mDirectLightingRenderer = new DirectLightingRenderer();
	mDirectLightingRenderer->Init();

	mDepthPyramidRenderer = new DepthPyramidRenderer();
	mDepthPyramidRenderer->Init(mGBuffer->GetWidth(), mGBuffer->GetHeight());

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


void DeferredRenderer::Render(std::vector<DrawableObject*> inDrawList)
{
	assert(mInitialized == true);
	GeometryPass(inDrawList);
	LightingPass();
}


void DeferredRenderer::GeometryPass(std::vector<DrawableObject*> inDrawList)
{
	std::vector<ID3D11RenderTargetView*> gbuffer_targets = mGBuffer->GetRenderTargetViewArray();
	
	ID3D11DeviceContext* context;
	theFramework.GetDevice()->GetImmediateContext(&context);

	for (int i = 0; i < GBuffer::NUM_RENDER_TARGETS; i++)
	context->ClearRenderTargetView(gbuffer_targets[i], DirectX::Colors::Red);

	ID3D11DepthStencilView* gbuffer_depth_stencil = mGBuffer->GetDepthStencilView();
	context->OMSetRenderTargets(GBuffer::NUM_RENDER_TARGETS, gbuffer_targets.data(), gbuffer_depth_stencil);
	context->ClearDepthStencilView(gbuffer_depth_stencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	for each (DrawableObject* obj in inDrawList)
		obj->Draw();

	// clear render targets
	ID3D11RenderTargetView* render_targets[] = { NULL, NULL, NULL, NULL, NULL };
	context->OMSetRenderTargets(5, render_targets, NULL);
}


void DeferredRenderer::LightingPass()
{
	//mShadowRenderer->Render(inDeviceContext);
	mDirectLightingRenderer->Render(mGBuffer, mDirectLighting);
	mDepthPyramidRenderer->Render(mGBuffer->GetRenderTarget(GBuffer::LINEAR_DEPTH)->GetTexture(), mDepthPyramid);
	//mIndirectLightingRenderer->Render(inDeviceContext);
	//mReflectionRenderer->Render(inDeviceContext);
	// volumetrics and particles
	// exposure
	//mPostProcessRenderer->Render(inDeviceContext);
}