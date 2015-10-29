#include "DeferredRenderer.h"
#include "RenderContext.h"
#include "Gaag.h"
#include "Camera.h"
#include "ConstantBuffer.h"
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
	assert(theRenderContext.IsInitialized());
	assert(inWidth > 0 && inHeight > 0);

	CleanUp();

	mGBuffer = std::make_shared<GBuffer>();
	mGBuffer->Init(inWidth, inHeight);

	mDepthPyramid = std::make_shared<RenderTarget>();
	mDepthPyramid->Init(inWidth / 2, inHeight / 2, 3, DXGI_FORMAT_R16G16_FLOAT);
	
	mDirectLightingDiffuse = std::make_shared<RenderTarget>();
	mDirectLightingDiffuse->Init(inWidth, inHeight, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);

	mDirectLightingSpecular = std::make_shared<RenderTarget>();
	mDirectLightingSpecular->Init(inWidth, inHeight, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);

	mIndirectLighting = std::make_shared<RenderTarget>();
	mIndirectLighting->Init(inWidth, inHeight, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);

	mReflections = std::make_shared<RenderTarget>();
	mReflections->Init(inWidth, inHeight, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);

	mLightComposed = std::make_shared<RenderTarget>();
	mLightComposed->Init(inWidth, inHeight, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);

	mAntiAliased = std::make_shared<RenderTarget>();
	mAntiAliased->Init(inWidth, inHeight, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);

	mAAHistoryFrame = std::make_shared<RenderTarget>();
	mAAHistoryFrame->Init(inWidth, inHeight, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);

	mPostProcessed = std::make_shared<RenderTarget>();
	mPostProcessed->Init(inWidth, inHeight, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);

	mShadowRenderer.Init();
	mDirectLightingRenderer.Init();
	mDepthPyramidRenderer.Init();
	mIndirectLightingRenderer.Init();
	mReflectionRenderer.Init();
	mLightComposeRenderer.Init();
	mTAARenderer.Init();
	mPostProcessRenderer.Init();
	
	mConstantBufferEveryFrame = std::make_shared<ConstantBuffer>();
	mConstantBufferEveryFrame->Init(sizeof(ConstantDataEveryFrame));

	mConstantBufferOnDemand = std::make_shared<ConstantBuffer>();
	mConstantBufferOnDemand->Init(sizeof(ConstantDataOnDemand));

	mInitialized = true;	
}


void DeferredRenderer::CleanUp()
{
	mGBuffer = nullptr;
	mDepthPyramid = nullptr;
	mDirectLightingDiffuse = nullptr;
	mDirectLightingSpecular = nullptr;
	mIndirectLighting = nullptr;
	mReflections = nullptr;
	mLightComposed = nullptr;
	mAntiAliased = nullptr;
	mPostProcessed = nullptr;
	mConstantBufferEveryFrame = nullptr;
	mConstantBufferOnDemand = nullptr;

	mShadowRenderer.CleanUp();
	mDirectLightingRenderer.CleanUp();
	mDepthPyramidRenderer.CleanUp();
	mIndirectLightingRenderer.CleanUp();
	mReflectionRenderer.CleanUp();
	mLightComposeRenderer.CleanUp();
	mTAARenderer.CleanUp();
	mPostProcessRenderer.CleanUp();
}


void DeferredRenderer::Render(std::vector<pDrawableObject> inDrawList)
{
	assert(mInitialized);
	
	Camera& camera = *(Gaag.GetCamera());
	int frame_id = Gaag.GetFrameID();
	float2 jitter_offset = TAARenderer::GetJitterOffset(frame_id);

	ConstantDataEveryFrame constantData;
	constantData.frameData = float4(jitter_offset.x, jitter_offset.y, frame_id, 0);
	constantData.viewMatrix = camera.GetViewMatrix();
	constantData.projectionMatrix = camera.GetProjectionMatrix();
	constantData.viewProjectionMatrix = camera.GetViewProjectionMatrix();
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(constantData.projectionMatrix);
	constantData.inverseProjectionMatrix = DirectX::XMMatrixInverse(&determinant, constantData.projectionMatrix);
	
	theRenderContext.UpdateSubResource(mConstantBufferEveryFrame, &constantData);

	GeometryPass(inDrawList);
	
	LightingPass();
	
	PostProcessPass();
}


void DeferredRenderer::GeometryPass(std::vector<pDrawableObject> inDrawList)
{
	theRenderContext.BeginEvent("Geometry Pass");
	
	for (int i = 0; i < GBuffer::NUM_RENDER_TARGETS; i++)
		theRenderContext.ClearRenderTarget(mGBuffer->GetRenderTarget(GBuffer::GBufferType(i)), float4(0, 0, 0, 0));
	
	theRenderContext.ClearDepthStencil(mGBuffer->GetDepthStencilTarget(), 1.0, 0);
	
	theRenderContext.SetRenderTargets(GBuffer::NUM_RENDER_TARGETS, mGBuffer->GetRenderTargets(), mGBuffer->GetDepthStencilTarget());

	for each (pDrawableObject obj in inDrawList)
	{
		theRenderContext.SetMarker("Drawing Object");

		Gaag.SetMaterial(obj->GetMaterial());

		theRenderContext.VSSetConstantBuffer(mConstantBufferEveryFrame, 0);
		theRenderContext.VSSetConstantBuffer(obj->GetConstantBuffer(), 1);

		theRenderContext.DrawMesh(obj->GetMesh());

		// reset state
		theRenderContext.VSSetConstantBuffer(NULL, 0);
		theRenderContext.PSSetConstantBuffer(NULL, 0);
		theRenderContext.PSSetTextureAndSampler(NULL, NULL, 0);
		theRenderContext.PSSetTextureAndSampler(NULL, NULL, 1);
		theRenderContext.PSSetTextureAndSampler(NULL, NULL, 2);
		theRenderContext.PSSetShader(NULL);
		theRenderContext.VSSetShader(NULL);
	}
	
	theRenderContext.Flush();

	// clear render targets
	pRenderTarget null_targets[] = { NULL, NULL, NULL, NULL, NULL };
	theRenderContext.SetRenderTargets(5, null_targets, NULL);
	theRenderContext.EndEvent();
}


void DeferredRenderer::LightingPass()
{
	theRenderContext.BeginEvent("Lighting Pass");
	
	theRenderContext.SetMarker("Shadow Renderer");
	//mShadowRenderer.Render();
	
	theRenderContext.SetMarker("Direct Lighting Renderer");
	mDirectLightingRenderer.Render(mGBuffer, mDirectLightingDiffuse, mDirectLightingSpecular);
	
	theRenderContext.SetMarker("Depth Pyramid Renderer");
	//mDepthPyramidRenderer.Render(mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH), mDepthPyramid);
	
	theRenderContext.SetMarker("Indirect Lighting Renderer");
	mIndirectLightingRenderer.Render(mDirectLightingDiffuse->GetTexture(), mGBuffer->GetTexture(GBuffer::NORMAL), 
		mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH), mIndirectLighting);
	
	theRenderContext.SetMarker("Reflection Renderer");
	mReflectionRenderer.Render(mIndirectLighting->GetTexture(), mReflections);

	theRenderContext.SetMarker("Light Compose Renderer");
	mLightComposeRenderer.Render(	mDirectLightingDiffuse->GetTexture(),
									mDirectLightingSpecular->GetTexture(),
									mIndirectLighting->GetTexture(),
									mReflections->GetTexture(),
									mLightComposed);
	theRenderContext.EndEvent();
}


void DeferredRenderer::PostProcessPass()
{
	theRenderContext.BeginEvent("Post Process Pass");
	mTAARenderer.Render(mLightComposed->GetTexture(), mAAHistoryFrame, mGBuffer->GetTexture(GBuffer::MOTION_VECTORS), mAntiAliased);
	mPostProcessRenderer.Render(mAntiAliased->GetTexture(), mGBuffer->GetRenderTarget(GBuffer::MOTION_VECTORS)->GetTexture(), mPostProcessed);
	theRenderContext.EndEvent();
}
