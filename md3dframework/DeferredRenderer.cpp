#include "DeferredRenderer.h"
#include "RenderContext.h"
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
#include <d3d11_1.h>


void DeferredRenderer::Init(int inWidth, int inHeight)
{
	assert(inWidth > 0 && inHeight > 0);

	CleanUp();

	mGBuffer = new GBuffer();
	mGBuffer->Init(inWidth, inHeight);

	mDepthPyramid = new RenderTarget();
	mDepthPyramid->Init(inWidth / 2, inHeight / 2, 3, DXGI_FORMAT_R16G16_FLOAT);
	
	mDirectLighting = new RenderTarget();
	mDirectLighting->Init(inWidth, inHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT);

	mIndirectLighting = new RenderTarget();
	mIndirectLighting->Init(inWidth, inHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT);

	mReflections = new RenderTarget();
	mReflections->Init(inWidth, inHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT);

	mLightComposed = new RenderTarget();
	mLightComposed->Init(inWidth, inHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT);

	mAntiAliased = new RenderTarget();
	mAntiAliased->Init(inWidth, inHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT);

	mAAHistoryFrame = new RenderTarget();
	mAAHistoryFrame->Init(inWidth, inHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT);

	mPostProcessed = new RenderTarget();
	mPostProcessed->Init(inWidth, inHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT);

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
	assert(mInitialized);
	GeometryPass(inDrawList);
	LightingPass();
	PostProcessPass();
}


void DeferredRenderer::GeometryPass(std::vector<DrawableObject*> inDrawList)
{
	theRenderContext.BeginEvent("Geometry Pass");
	
	for (int i = 0; i < GBuffer::NUM_RENDER_TARGETS; i++)
		theRenderContext.ClearRenderTarget(mGBuffer->GetRenderTarget(GBuffer::GBufferType(i)), float4(0, 0, 0, 0));
	
	theRenderContext.ClearDepthStencil(mGBuffer->GetDepthStencilTarget(), 1.0, 0);
	
	theRenderContext.SetRenderTargets(GBuffer::NUM_RENDER_TARGETS, mGBuffer->GetRenderTargets(), mGBuffer->GetDepthStencilTarget());

	for each (DrawableObject* obj in inDrawList)
	{
		theRenderContext.SetMarker("Drawing Object");
		obj->Draw();
	}

	// clear render targets
	RenderTarget* null_targets[] = { NULL, NULL, NULL, NULL, NULL };
	theRenderContext.SetRenderTargets(5, null_targets, NULL);
	theRenderContext.EndEvent();
}


void DeferredRenderer::LightingPass()
{
	theRenderContext.BeginEvent("Lighting Pass");
	
	theRenderContext.SetMarker("Shadow Renderer");
	//mShadowRenderer->Render();
	
	theRenderContext.SetMarker("Direct Lighting Renderer");
	mDirectLightingRenderer->Render(mGBuffer, mDirectLighting);
	
	theRenderContext.SetMarker("Depth Pyramid Renderer");
	//mDepthPyramidRenderer->Render(mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH), mDepthPyramid);
	
	theRenderContext.SetMarker("Indirect Lighting Renderer");
	mIndirectLightingRenderer->Render(mDirectLighting->GetTexture(), mGBuffer->GetTexture(GBuffer::NORMAL), 
		mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH), mIndirectLighting);
	
	theRenderContext.SetMarker("Reflection Renderer");
	mReflectionRenderer->Render(mIndirectLighting->GetTexture(), mReflections);

	theRenderContext.SetMarker("Light Compose Renderer");
	mLightComposeRenderer->Render(	mDirectLighting->GetTexture(),
									mIndirectLighting->GetTexture(),
									mReflections->GetTexture(),
									mLightComposed);
	theRenderContext.EndEvent();
}


void DeferredRenderer::PostProcessPass()
{
	theRenderContext.BeginEvent("Post Process Pass");
	mTAARenderer->Render(mLightComposed->GetTexture(), mAAHistoryFrame, mGBuffer->GetTexture(GBuffer::MOTION_VECTORS), mAntiAliased);
	mPostProcessRenderer->Render(mAntiAliased->GetTexture(), mGBuffer->GetRenderTarget(GBuffer::MOTION_VECTORS)->GetTexture(), mPostProcessed);
	theRenderContext.EndEvent();
}