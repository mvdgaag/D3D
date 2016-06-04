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
#include "MeshObject.h"
#include "PointLight.h"
#include "TextureUtil.h"
#include <assert.h>
#include <d3d11_1.h>


#define HALF_RES_INDIRECT


void DeferredRenderer::Init(int inWidth, int inHeight)
{
	assert(theRenderContext.IsInitialized());
	assert(mInitialized == false);
	assert(inWidth > 0 && inHeight > 0);

	mGBuffer = MAKE_NEW(GBuffer);
	mGBuffer->Init(inWidth, inHeight);

	mDepthMinPyramid = theResourceFactory.MakeRenderTarget(int2(inWidth / 2, inHeight / 2), 8, FORMAT_R16_FLOAT);
	mDepthMaxPyramid = theResourceFactory.MakeRenderTarget(int2(inWidth / 2, inHeight / 2), 8, FORMAT_R16_FLOAT);
	mDirectLightingDiffuse = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mDirectLightingDiffuseTemp = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mDirectLightingSpecular = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mDirectLightingSpecularTemp = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
#ifdef HALF_RES_INDIRECT
	mIndirectLighting = theResourceFactory.MakeRenderTarget(int2(inWidth / 2, inHeight / 2), 1, FORMAT_R16G16B16A16_FLOAT);
#else
	mIndirectLighting = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
#endif
	mReflections = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mLightComposed = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mAntiAliased = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mAAHistoryFrame = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mPostProcessed = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mFullResRGBATemp = theResourceFactory.MakeRenderTarget(int2(inWidth, inHeight), 1, FORMAT_R16G16B16A16_FLOAT);
	mHalfResRGBATemp = theResourceFactory.MakeRenderTarget(int2(inWidth / 2, inHeight / 2), 1, FORMAT_R16G16B16A16_FLOAT);
	mQuarterResRGBATemp = theResourceFactory.MakeRenderTarget(int2(inWidth / 4, inHeight / 4), 1, FORMAT_R16G16B16A16_FLOAT);
	mEightResRGBATemp = theResourceFactory.MakeRenderTarget(int2(inWidth / 8, inHeight / 8), 1, FORMAT_R16G16B16A16_FLOAT);
	mHalfLinearDepth = theResourceFactory.MakeRenderTarget(int2(inWidth / 2, inHeight / 2), 1, FORMAT_R32_FLOAT);
	mHalfNormals = theResourceFactory.MakeRenderTarget(int2(inWidth / 2, inHeight / 2), 1, FORMAT_R16G16_FLOAT);
	
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
	theResourceFactory.DestroyItem(mDirectLightingDiffuseTemp);
	theResourceFactory.DestroyItem(mDirectLightingSpecular);
	theResourceFactory.DestroyItem(mDirectLightingSpecularTemp);
	theResourceFactory.DestroyItem(mIndirectLighting);
	theResourceFactory.DestroyItem(mReflections);
	theResourceFactory.DestroyItem(mLightComposed);
	theResourceFactory.DestroyItem(mAntiAliased);
	theResourceFactory.DestroyItem(mAAHistoryFrame);
	theResourceFactory.DestroyItem(mPostProcessed);

	theResourceFactory.DestroyItem(mFullResRGBATemp);
	theResourceFactory.DestroyItem(mHalfResRGBATemp);
	theResourceFactory.DestroyItem(mQuarterResRGBATemp);
	theResourceFactory.DestroyItem(mEightResRGBATemp);
	
	theResourceFactory.DestroyItem(mConstantBufferEveryFrame);
	theResourceFactory.DestroyItem(mConstantBufferEveryObject);
	theResourceFactory.DestroyItem(mConstantBufferOnDemand);

	mGBuffer = nullptr;
	mDepthMinPyramid = nullptr;
	mDepthMaxPyramid = nullptr;
	mDirectLightingDiffuse = nullptr;
	mDirectLightingDiffuseTemp = nullptr;
	mDirectLightingSpecular = nullptr;
	mDirectLightingSpecularTemp = nullptr;
	mIndirectLighting = nullptr;
	mReflections = nullptr;
	mLightComposed = nullptr;
	mAntiAliased = nullptr;
	mAAHistoryFrame = nullptr;
	mPostProcessed = nullptr;
	mFullResRGBATemp = nullptr;
	mHalfResRGBATemp = nullptr;
	mQuarterResRGBATemp = nullptr;
	mEightResRGBATemp = nullptr;
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



void DeferredRenderer::Render(std::vector<pMeshObject> inDrawList)
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


pTexture DeferredRenderer::PreFilterCubemap(pTexture inCubemap)
{
	assert(mInitialized);
	return mReflectionRenderer.FilterCubemap(inCubemap);
}


void DeferredRenderer::GeometryPass(std::vector<pMeshObject> inDrawList)
{
	theRenderContext.BeginEvent("Geometry Pass");

	theRenderContext.SetRasterizerState(FILL_SOLID, CULL_BACK, true, 0, 0.0f, 0.0f, true, false, false, false);
	theRenderContext.SetViewport(int2(mGBuffer->GetWidth(), mGBuffer->GetHeight()), 0.0f, 1.0f, int2(0, 0));
	
	for (int i = 0; i < GBuffer::NUM_RENDER_TARGETS; i++)
		theRenderContext.ClearRenderTarget(mGBuffer->GetRenderTarget(GBuffer::GBufferType(i)), mGBuffer->GetClearColor(GBuffer::GBufferType(i)));
	
	theRenderContext.ClearDepthStencil(mGBuffer->GetDepthStencilTarget(), 1.0, 0);
	theRenderContext.SetRenderTargets(GBuffer::NUM_RENDER_TARGETS, mGBuffer->GetRenderTargets(), mGBuffer->GetDepthStencilTarget());

	Camera& camera = *(Gaag.GetCamera());
	Frustum camera_frustum = camera.ExtractFrustum();
	for each (pMeshObject obj in inDrawList)
	{
		AABB& aabb = obj->GetAABB();
		float3 position = obj->GetPosition() + aabb.mCenter;
		if (!camera_frustum.TestSphere(position, aabb.mRadius))
			continue;

		theRenderContext.SetMarker("Drawing Object");

		ConstantDataEveryObject constantData;
		Camera& camera = *(Gaag.GetCamera());
		constantData.modelViewMatrix = transpose(camera.GetViewMatrix() * obj->GetTransform());
		constantData.modelViewProjectionMatrix = transpose(camera.GetViewProjectionMatrix() * obj->GetTransform());
		constantData.PrevModelViewProjectionMatrix = transpose(mPrevViewProjectionMatrix * obj->GetPrevTransform());

		theRenderContext.UpdateSubResource(*mConstantBufferEveryObject, &constantData);

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

	// render shadow maps
	for (int i = 0; i < mDirectionalLights.size(); i++)
	{
		theRenderContext.SetMarker("Shadow Pass");
		if (mDirectionalLights[i]->GetShadowMap() != nullptr)
			mShadowRenderer.Render(mDirectionalLights[i], inDrawList);
	}

	// swap the transforms
	for each (pMeshObject obj in inDrawList)
		obj->SwapTransform();

	// store projection matrix at previous projection matrix
	mPrevViewProjectionMatrix = Gaag.GetCamera()->GetViewProjectionMatrix();
}


void DeferredRenderer::LightingPass()
{
	theRenderContext.BeginEvent("Lighting Pass");
	
	if (mRenderState.EnableDirect)
	{
		theRenderContext.SetMarker("Direct Lighting Renderer");
		mDirectLightingRenderer.Render(mGBuffer, mDirectLightingDiffuse, mDirectLightingSpecular, mDirectLightingDiffuseTemp, mDirectLightingSpecularTemp,
			mPointLights, mSpotLights, mDirectionalLights);
	}

	if (mRenderState.EnableIndirect || mRenderState.EnableReflections)
	{
		theRenderContext.SetMarker("Depth Pyramid Renderer");
		mDepthPyramidRenderer.Render(mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH), mDepthMaxPyramid, mDepthMinPyramid);
	}

	if (mRenderState.EnableIndirect)
	{
		theRenderContext.SetMarker("Downres Diffuse Lighting");
		TextureUtil::TextureDownSample(mHalfResRGBATemp, mDirectLightingDiffuse->GetTexture(), theResourceFactory.GetDefaultLinearSampler());
		TextureUtil::TextureDownSample(mQuarterResRGBATemp, mHalfResRGBATemp->GetTexture(), theResourceFactory.GetDefaultLinearSampler());
		TextureUtil::TextureDownSample(mEightResRGBATemp, mQuarterResRGBATemp->GetTexture(), theResourceFactory.GetDefaultLinearSampler());

#ifdef HALF_RES_INDIRECT
		theRenderContext.SetMarker("Downres depth and normals");
		// downsample depth
		TextureUtil::TextureDownSample(mHalfLinearDepth, mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH), theResourceFactory.GetDefaultPointSampler());
		// downsample normals
		TextureUtil::TextureDownSample(mHalfNormals, mGBuffer->GetTexture(GBuffer::NORMAL), theResourceFactory.GetDefaultPointSampler());
		// downsample diffuse light
		theRenderContext.SetMarker("Indirect Lighting Renderer");
		mIndirectLightingRenderer.Render(mEightResRGBATemp->GetTexture(), mHalfNormals->GetTexture(),
			mHalfLinearDepth->GetTexture(), mDepthMaxPyramid->GetTexture(),
			mGBuffer->GetTexture(GBuffer::DIFFUSE), mIndirectLighting, mFullResRGBATemp);
#else
		theRenderContext.SetMarker("Indirect Lighting Renderer");
		mIndirectLightingRenderer.Render(mEightResRGBATemp->GetTexture(), mGBuffer->GetTexture(GBuffer::NORMAL), mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH), mGBuffer->GetTexture(GBuffer::DIFFUSE), mIndirectLighting, mFullResRGBATemp);
#endif
	}

	if (mRenderState.EnableReflections && mRenderState.CubeMap != nullptr)
	{
		theRenderContext.SetMarker("Reflection Renderer");
		mReflectionRenderer.Render(mIndirectLighting->GetTexture(), mReflections, mGBuffer->GetTexture(GBuffer::NORMAL), mGBuffer->GetTexture(GBuffer::MATERIAL), 
			mGBuffer->GetTexture(GBuffer::DIFFUSE), mDepthMinPyramid->GetTexture(), mRenderState.CubeMap);
	}

	theRenderContext.SetMarker("Light Compose Renderer");
	mLightComposeRenderer.Render(	mDirectLightingDiffuse->GetTexture(),
									mDirectLightingSpecular->GetTexture(),
									mIndirectLighting->GetTexture(),
									mReflections->GetTexture(),
									mGBuffer->GetTexture(GBuffer::LINEAR_DEPTH),
									mHalfLinearDepth->GetTexture(),
									mLightComposed);
									
	theRenderContext.EndEvent();
}


void DeferredRenderer::PostProcessPass()
{
	theRenderContext.BeginEvent("Post Process Pass");
	theRenderContext.SetMarker("Post Process Shader");
	mPostProcessRenderer.Render(mLightComposed->GetTexture(), mGBuffer->GetRenderTarget(GBuffer::MOTION_VECTORS)->GetTexture(), mPostProcessed);
	
	// temporal AA should be very last thing, because it clamps the values
	theRenderContext.SetMarker("Temporal AA Shader");
	mTAARenderer.Render(mPostProcessed->GetTexture(), mAAHistoryFrame, mGBuffer->GetTexture(GBuffer::MOTION_VECTORS), mAntiAliased);
	theRenderContext.EndEvent();
}
