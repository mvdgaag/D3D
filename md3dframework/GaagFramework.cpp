#include "GaagFramework.h"
#include "RenderContext.h"
#include "ResourceFactory.h"
#include "TextureUtil.h"
#include "DeferredRenderer.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "GBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "Sampler.h"
#include "ConstantBuffer.h"
#include "Mesh.h"
#include "Material.h"
#include "Time.h"

// crt_dupenv_s.c  
#include  <stdlib.h>  


GaagFramework::~GaagFramework()
{
	CleanUp();
}


HRESULT GaagFramework::Init(HWND inHWND, int inWidth, int inHeight)
{
	CleanUp();
	assert(mInitialized == false);

	char *pValue;
	size_t len;
	getenv_s(&len, nullptr, 0, "GAAGDIR");
	
	assert(len != 0);
	if (len != 0)
	{
		pValue = (char*)malloc(len * sizeof(char));
		getenv_s(&len, pValue, len, "GAAGDIR");
		assert(pValue != nullptr);
		mDirectory = pValue;
		free(pValue);
	}

	assert(mDirectory != "");

	theRenderContext.Init(inHWND, inWidth, inHeight);
	theResourceFactory.Init();
	theTime.Init();
	theInput.Init();
	TextureUtil::InitTextureUtil();

	mDeferredRenderer = MAKE_NEW(DeferredRenderer);
	mDeferredRenderer->Init(theRenderContext.GetWidth(), theRenderContext.GetHeight());

	mFullScreenTriangle = theResourceFactory.GetFullScreenTriangleMesh();
	mDefaultSampler = theResourceFactory.GetDefaultLinearSampler();
	mCopyShader = theResourceFactory.GetCopyShader();
	mCopyConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(float4));

	mCamera = MAKE_NEW(Camera);
	mCamera->SetPosition(1.0, 1.0, 1.0);
	mCamera->SetTarget(0.0, 0.0, 0.0);
	mCamera->SetWorldUp(0.0, 1.0, 0.0);

	mFrameID = 0;
	mFrameTime = theTime.GetTime();

	mInitialized = true;
	return S_OK;
}


HRESULT GaagFramework::Init(HINSTANCE hInstance, int inWidth, int inHeight)
{
	mWindow = MAKE_NEW(Window);
	mWindow->Init(hInstance, inWidth, inHeight);
	return Init(mWindow);
}


HRESULT GaagFramework::Init(pWindow inWindow)
{
	mWindow = inWindow;
	RECT rc;
	GetClientRect(mWindow->GetHandle(), &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	return Init(inWindow->GetHandle(), width, height);
}


void GaagFramework::CleanUp()
{
	if (mInitialized == false)
		return;

	if (mCamera)
	{
		delete mCamera;
		mCamera = nullptr;
	}
	if (mDeferredRenderer)
	{
		delete mDeferredRenderer;
		mDeferredRenderer = nullptr;
	}
	theResourceFactory.DestroyItem(mFullScreenTriangle);
	theResourceFactory.DestroyItem(mDefaultSampler);
	theResourceFactory.DestroyItem(mCopyShader);
	theResourceFactory.DestroyItem(mCopyConstantBuffer);

	TextureUtil::CleanUpTextureUtil();
	theInput.CleanUp();
	theTime.CleanUp();
	theRenderContext.CleanUp();
	mWindow = nullptr;

	mInitialized = false;
}


void GaagFramework::Resize(int inWidth, int inHeight)
{
	assert(mInitialized);
	theRenderContext.Resize(inWidth, inHeight);
	mDeferredRenderer->Resize(inWidth, inHeight);
}


void GaagFramework::Render()
{
	assert(mInitialized);
	
	double now = theTime.GetTime();
	mDeltaTime = now - mFrameTime;
	mFrameTime = now;
	mFrameID++;

	if (mFrameCallback != nullptr)
		mFrameCallback();

	mDeferredRenderer->Render(mObjectList);
	CopyToRenderTarget(theRenderContext.GetOutputRenderTarget(), mDeferredRenderer->GetAntiAliased()->GetTexture(), 0);
	
	// HACK
	//CopyToRenderTarget(theRenderContext.GetOutputRenderTarget(), mDeferredRenderer->GetDirectLightingDiffuse()->GetTexture(), 0);
	//CopyToRenderTarget(theRenderContext.GetOutputRenderTarget(), mDeferredRenderer->GetGBuffer()->GetTexture(GBuffer::NORMAL), 0);
	
	theRenderContext.SwapBuffers();
}


float3 GaagFramework::ScreenToCameraPos(int2 inScreenPos)
{
	pTexture linear_depth_texture = mDeferredRenderer->GetGBuffer()->GetTexture(GBuffer::LINEAR_DEPTH);
	int2 dimensions = linear_depth_texture->GetDimensions();
	float2 NDC = float2(		float(inScreenPos.x) / dimensions.x, 
						  1.0 -	float(inScreenPos.y) / dimensions.y ) * 2.0f - 1.0f;
	float linear_depth = linear_depth_texture->GetPixel(inScreenPos).x;
	float2 view_reconstruct = float2(tan(0.5f * mCamera->GetFovX()), tan(0.5f * mCamera->GetFovY()));
	float2 xy = view_reconstruct * NDC * linear_depth;
	float3 cam_pos = float3(xy.x, xy.y, -linear_depth);
	return cam_pos;
}


float3 GaagFramework::WorldToCameraPos(float3 inCameraPos)
{
	// TODO: makes it all 0?
	float4x4 view = mCamera->GetViewMatrix();
	float4 camera_pos = view * float4(inCameraPos, 1.0);
	return float3(camera_pos);
}


float3 GaagFramework::CameraToWorldPos(float3 inCameraPos)
{
	float4x4 inverse_view = inverse(mCamera->GetViewMatrix());
	float4 world_pos = inverse_view * float4(inCameraPos, 1.0);
	return float3(world_pos);
}


float3 GaagFramework::WorldToCameraNormal(float3 inCameraNormal)
{
	// TODO: makes it all 0
	float4x4 view = mCamera->GetViewMatrix();
	float4 camera_normal = view * float4(inCameraNormal, 0.0);
	return float3(camera_normal);
}


float3 GaagFramework::CameraToWorldNormal(float3 inCameraNormal)
{
	float4x4 inverse_view = inverse(mCamera->GetViewMatrix());
	float4 world_normal = inverse_view * float4(inCameraNormal, 0.0);
	return float3(world_normal);
}


float3 GaagFramework::ScreenToWorldPos(int2 inScreenPos)
{
	float3 cam_pos = ScreenToCameraPos(inScreenPos);
	float3 world_pos = CameraToWorldPos(cam_pos);
	return world_pos;
}


void GaagFramework::SetMaterial(pMaterial inMaterial)
{
	theRenderContext.PSSetShader(inMaterial->GetPixelShader());
	theRenderContext.VSSetShader(inMaterial->GetVertexShader());
	
	pTexture diffuse_texture = inMaterial->GetDiffuseTexture();
	pTexture normal_texture = inMaterial->GetNormalTexture();
	pTexture surface_texture = inMaterial->GetSurfaceTexture();
	
	if (diffuse_texture != nullptr)
		theRenderContext.PSSetTextureAndSampler(diffuse_texture, mDefaultSampler, 0);
	if (normal_texture != nullptr)
		theRenderContext.PSSetTextureAndSampler(normal_texture, mDefaultSampler, 1);
	if (surface_texture != nullptr)
		theRenderContext.PSSetTextureAndSampler(surface_texture, mDefaultSampler, 2);
	
	theRenderContext.PSSetConstantBuffer(inMaterial->GetConstantBuffer(), 1); // 0 reserved for every frame
}


void GaagFramework::CopyToRenderTarget(pRenderTarget inTarget, pTexture inSource, int inMipLevel)
{
	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	float4 constant_data = float4(inTarget->GetDimensions(), inMipLevel, 0);
	theRenderContext.UpdateSubResource(*mCopyConstantBuffer, &constant_data);

	theRenderContext.CSSetShader(mCopyShader);
	theRenderContext.CSSetTextureAndSampler(inSource, theResourceFactory.GetDefaultPointSampler(), 0);
	theRenderContext.CSSetRWTexture(inTarget, 0, 0);
	theRenderContext.CSSetConstantBuffer(mCopyConstantBuffer, 0);

	int2 groups = (inTarget->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(groups.x, groups.y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
}
