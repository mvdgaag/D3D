#include "DeferredRenderer.h"
#include "Framework.h"
#include "GBuffer.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "ShadowRenderer.h"
#include "DirectLightingRenderer.h"
#include "DepthPyramidRenderer.h"
#include "IndirectLightingRenderer.h"
#include "ReflectionRenderer.h"
#include "LightComposeRenderer.h"
#include "TAARenderer.h"
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

	mIndirectLighting = new RenderTarget("IndirectLightingRenderTarget");
	mIndirectLighting->Init(inWidth, inHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mReflections = new RenderTarget("ReflectionsRenderTarget");
	mReflections->Init(inWidth, inHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mLightComposed = new RenderTarget("LightComposedRenderTarget");
	mLightComposed->Init(inWidth, inHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mAntiAliased = new RenderTarget("AntiAliasedRenderTarget");
	mAntiAliased->Init(inWidth, inHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mAAHistoryFrame = new RenderTarget("AntiAliasHistoryRenderTarget");
	mAAHistoryFrame->Init(inWidth, inHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

	mPostProcessed = new RenderTarget("PostProcessedRenderTarget");
	mPostProcessed->Init(inWidth, inHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

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

	mLightComposeRenderer = new LightComposeRenderer();
	mLightComposeRenderer->Init();

	mTAARenderer = new TAARenderer();
	mTAARenderer->Init();

	mPostProcessRenderer = new PostProcessRenderer();
	mPostProcessRenderer->Init();

	mInitialized = true;	
}


void DeferredRenderer::CleanUp()
{
	delete mGBuffer;
	mGBuffer = NULL;

	delete mDepthPyramid;
	mDepthPyramid = NULL;
	delete mDirectLighting;
	mDirectLighting = NULL;
	delete mIndirectLighting;
	mIndirectLighting = NULL;
	delete mReflections;
	mReflections = NULL;
	delete mLightComposed;
	mLightComposed = NULL;
	delete mAntiAliased;
	mAntiAliased = NULL;
	delete mPostProcessed;
	mPostProcessed = NULL;

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
	delete mLightComposeRenderer;
	mLightComposeRenderer = NULL;
	delete mTAARenderer;
	mTAARenderer = NULL;
	delete mPostProcessRenderer;
	mPostProcessRenderer = NULL;
}


void DeferredRenderer::Render(std::vector<DrawableObject*> inDrawList)
{
	assert(mInitialized == true);
	GeometryPass(inDrawList);
	LightingPass();
	PostProcessPass();
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
	//mShadowRenderer->Render();
	mDirectLightingRenderer->Render(mGBuffer, mDirectLighting);
	mDepthPyramidRenderer->Render(mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH), mDepthPyramid);
	mIndirectLightingRenderer->Render(mDirectLighting->GetTexture(), mIndirectLighting);
	mReflectionRenderer->Render(mIndirectLighting->GetTexture(), mReflections);
	mLightComposeRenderer->Render(	mDirectLighting->GetTexture(),
									mIndirectLighting->GetTexture(),
									mReflections->GetTexture(),
									mLightComposed);
}


void DeferredRenderer::PostProcessPass()
{
	mTAARenderer->Render(mLightComposed->GetTexture(), mAAHistoryFrame, mGBuffer->GetTexture(GBuffer::MOTION_VECTORS), mAntiAliased);
	mPostProcessRenderer->Render(mAntiAliased->GetTexture(), mPostProcessed);
}