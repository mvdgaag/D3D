#include "DeferredRenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
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
#include "PointLight.h"
#include <assert.h>
#include <d3d11_1.h>


void DeferredRenderer::Init(int inWidth, int inHeight)
{
	assert(theRenderContext.IsInitialized());
	assert(mInitialized == false);
	assert(inWidth > 0 && inHeight > 0);

	mGBuffer = MAKE_NEW(GBuffer);
	mGBuffer->Init(inWidth, inHeight);

	mDepthMinPyramid = theResourceFactory.MakeRenderTarget(int2(inWidth / 2, inHeight / 2), mDepthPyramidRenderer.GetNumMipLevels(), FORMAT_R16_FLOAT);
	mDepthMaxPyramid = theResourceFactory.MakeRenderTarget(int2(inWidth / 2, inHeight / 2), mDepthPyramidRenderer.GetNumMipLevels(), FORMAT_R16_FLOAT);
	mDirectLightingDiffuse = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mDirectLightingSpecular = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mIndirectLighting = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mReflections = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mLightComposed = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mAntiAliased = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mAAHistoryFrame = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mPostProcessed = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mFullResRGBATemp = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	
	mConstantBufferEveryFrame = theResourceFactory.MakeConstantBuffer(sizeof(ConstantDataEveryFrame));
	mConstantBufferEveryObject = theResourceFactory.MakeConstantBuffer(sizeof(ConstantDataEveryObject));
	mConstantBufferOnDemand = theResourceFactory.MakeConstantBuffer(sizeof(ConstantDataOnDemand));

	mShadowRenderer.Init();
	mDirectLightingRenderer.Init();
	mDepthPyramidRenderer.Init();
	mIndirectLightingRenderer.Init();
	mReflectionRenderer.Init();
	mLightComposeRenderer.Init();
	mTAARenderer.Init();
	mPostProcessRenderer.Init();

	mInitialized = true;	
}


void DeferredRenderer::CleanUp()
{
	delete mGBuffer;
	theResourceFactory.DestroyItem(mDepthMinPyramid);
	theResourceFactory.DestroyItem(mDepthMaxPyramid);
	theResourceFactory.DestroyItem(mDirectLightingDiffuse);
	theResourceFactory.DestroyItem(mDirectLightingSpecular);
	theResourceFactory.DestroyItem(mIndirectLighting);
	theResourceFactory.DestroyItem(mReflections);
	theResourceFactory.DestroyItem(mLightComposed);
	theResourceFactory.DestroyItem(mAntiAliased);
	theResourceFactory.DestroyItem(mAAHistoryFrame);
	theResourceFactory.DestroyItem(mPostProcessed);
	theResourceFactory.DestroyItem(mFullResRGBATemp);
	theResourceFactory.DestroyItem(mConstantBufferEveryFrame);
	theResourceFactory.DestroyItem(mConstantBufferEveryObject);
	theResourceFactory.DestroyItem(mConstantBufferOnDemand);

	mGBuffer = nullptr;
	mDepthMinPyramid = nullptr;
	mDepthMaxPyramid = nullptr;
	mDirectLightingDiffuse = nullptr;
	mDirectLightingSpecular = nullptr;
	mIndirectLighting = nullptr;
	mReflections = nullptr;
	mLightComposed = nullptr;
	mAntiAliased = nullptr;
	mAAHistoryFrame = nullptr;
	mPostProcessed = nullptr;
	mFullResRGBATemp = nullptr;

	mConstantBufferEveryFrame = nullptr;
	mConstantBufferEveryObject = nullptr;
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
	constantData.viewMatrix = lookAt(camera.GetPosition(), float3(0.0), float3(0.0, 1.0, 0.0));
	constantData.projectionMatrix = perspective(camera.GetFovY(), camera.GetAspect(), camera.GetNear(), camera.GetFar());
	constantData.viewProjectionMatrix = constantData.projectionMatrix * constantData.viewMatrix;
	constantData.inverseProjectionMatrix = inverse(constantData.projectionMatrix);
	constantData.frameData = float4(jitter_offset.x, jitter_offset.y, frame_id, 0);

	theRenderContext.UpdateSubResource(*mConstantBufferEveryFrame, &constantData);

	GeometryPass(inDrawList);
	
	LightingPass();
	
	PostProcessPass();
}


void DeferredRenderer::RegisterLight(pPointLight inLight)
{
	mPointLights.push_back(inLight);
}


void DeferredRenderer::RegisterLight(pSpotLight inLight)
{
	mSpotLights.push_back(inLight);
}


void DeferredRenderer::RegisterLight(pDirectionalLight inLight)
{
	mDirectionalLights.push_back(inLight);
}


void DeferredRenderer::ClearLights()
{
	mPointLights.clear(); 
	mSpotLights.clear();
}


void DeferredRenderer::GeometryPass(std::vector<pDrawableObject> inDrawList)
{
	theRenderContext.BeginEvent("Geometry Pass");
	
	for (int i = 0; i < GBuffer::NUM_RENDER_TARGETS; i++)
		theRenderContext.ClearRenderTarget(mGBuffer->GetRenderTarget(GBuffer::GBufferType(i)), mGBuffer->GetClearColor(GBuffer::GBufferType(i)));
	
	theRenderContext.ClearDepthStencil(mGBuffer->GetDepthStencilTarget(), 1.0, 0);
	
	theRenderContext.SetRenderTargets(GBuffer::NUM_RENDER_TARGETS, mGBuffer->GetRenderTargets(), mGBuffer->GetDepthStencilTarget());

	for each (pDrawableObject obj in inDrawList)
	{
		theRenderContext.SetMarker("Drawing Object");

		ConstantDataEveryObject constantData;
		Camera& camera = *(Gaag.GetCamera());
		constantData.modelViewMatrix = transpose(camera.GetViewMatrix() * obj->GetTransform());
		constantData.modelViewProjectionMatrix = transpose(camera.GetViewProjectionMatrix() * obj->GetTransform());
		constantData.PrevModelViewProjectionMatrix = transpose(mPrevViewProjectionMatrix * obj->GetPrevTransform());

		theRenderContext.UpdateSubResource(*mConstantBufferEveryObject, &constantData);
		obj->SwapTransform();

		Gaag.SetMaterial(obj->GetMaterial());
		
		theRenderContext.VSSetConstantBuffer(mConstantBufferEveryFrame, 0);
		theRenderContext.VSSetConstantBuffer(mConstantBufferEveryObject, 1);
		obj->PrepareToDraw();
		theRenderContext.DrawMesh(*(obj->GetMesh()));
		obj->FinalizeAfterDraw();

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

	mPrevViewProjectionMatrix = Gaag.GetCamera()->GetViewProjectionMatrix();
}


void DeferredRenderer::LightingPass()
{
	theRenderContext.BeginEvent("Lighting Pass");
	
	//theRenderContext.SetMarker("Shadow Renderer");
	//mShadowRenderer.Render();
	
	theRenderContext.SetMarker("Direct Lighting Renderer");
	mDirectLightingRenderer.Render(mGBuffer, mDirectLightingDiffuse, mDirectLightingSpecular, mPointLights, mSpotLights, mDirectionalLights);
	
	theRenderContext.SetMarker("Depth Pyramid Renderer");
	mDepthPyramidRenderer.Render(mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH), mDepthMaxPyramid, mDepthMinPyramid);
	
	theRenderContext.SetMarker("Indirect Lighting Renderer");
	mIndirectLightingRenderer.Render(mDirectLightingDiffuse->GetTexture(), mGBuffer->GetTexture(GBuffer::NORMAL), mGBuffer->GetTexture(GBuffer::DIFFUSE),
		mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH), mDepthMaxPyramid->GetTexture(), mIndirectLighting, mFullResRGBATemp);
	
	//theRenderContext.SetMarker("Reflection Renderer");
	//mReflectionRenderer.Render(mIndirectLighting->GetTexture(), mReflections);

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
	mPostProcessRenderer.Render(mLightComposed->GetTexture(), mGBuffer->GetRenderTarget(GBuffer::MOTION_VECTORS)->GetTexture(), mPostProcessed);
	
	// temporal AA should be very last thing, because it clamps the values
	mTAARenderer.Render(mPostProcessed->GetTexture(), mAAHistoryFrame, mGBuffer->GetTexture(GBuffer::MOTION_VECTORS), mAntiAliased);
	theRenderContext.EndEvent();
}
