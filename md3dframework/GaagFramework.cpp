#include "GaagFramework.h"
#include "RenderContext.h"
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


GaagFramework::~GaagFramework()
{
	CleanUp();
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT GaagFramework::Init(HINSTANCE hInstance)
{
	mWindow = MAKE_NEW(Window);
	mWindow->Init(hInstance);
	theRenderContext.Init(mWindow);
	TextureUtil::InitTextureUtil();
	
	mDeferredRenderer = MAKE_NEW(DeferredRenderer);
	mDeferredRenderer->Init(theRenderContext.GetWidth(), theRenderContext.GetHeight());

	mFullScreenTriangle = MAKE_NEW(Mesh);
	mFullScreenTriangle->InitFullscreenTriangle();

	mCopyShader = MAKE_NEW(ComputeShader);
	mCopyShader->InitFromFile("../md3dFramework/Shaders/CopyCompute.hlsl");

	mDefaultPointSampler = MAKE_NEW(Sampler);
	mDefaultPointSampler->Init(0); // D3D11_FILTER_MIN_MAG_MIP_POINT

	mDefaultLinearSampler = MAKE_NEW(Sampler);
	mDefaultLinearSampler->Init(21); // D3D11_FILTER_MIN_MAG_MIP_LINEAR

	mCamera = MAKE_NEW(Camera);
	mCamera->SetPosition(0.0, 20.0, -25.0);
	mCamera->SetTarget(0.0, 0.0, 0.0);
	mCamera->SetUp(0.0, 1.0, 0.0);

	mFrameID = 0;
	mFrameTime = theTime.GetTime();

	mInitialized = true;
	return S_OK;
}


void GaagFramework::CleanUp()
{
	mCamera = nullptr;
	mDeferredRenderer = nullptr;
	mFullScreenTriangle = nullptr;
	mCopyShader = nullptr;
	mDefaultPointSampler = nullptr;
	mDefaultLinearSampler = nullptr;

	TextureUtil::CleanUpTextureUtil();
	theRenderContext.CleanUp();
	mWindow = nullptr;
	
	mInitialized = false;
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
	CopyToRenderTarget(theRenderContext.GetOutputRenderTarget(), mDeferredRenderer->GetPostProcessed()->GetTexture());
	theRenderContext.SwapBuffers();
}


float3 GaagFramework::ScreenToCameraPos(int2 inScreenPos)
{
	pTexture linear_depth_texture = mDeferredRenderer->GetGBuffer()->GetTexture(GBuffer::LINEAR_DEPTH);
	float2 NDC = float2(		float(inScreenPos.x) / linear_depth_texture->GetWidth(), 
						  1.0 -	float(inScreenPos.y) / linear_depth_texture->GetHeight() ) * 2.0 - 1.0;
	float linear_depth = linear_depth_texture->GetPixel(inScreenPos).x;
	float2 view_reconstruct = float2(tan(0.5 * mCamera->GetFovX()), tan(0.5 * mCamera->GetFovY()));
	float2 xy = view_reconstruct * NDC * linear_depth;
	float3 cam_pos = float3(xy.x, xy.y, -linear_depth);
	return cam_pos;
}


float3 GaagFramework::CameraToWorldPos(float3 inCameraPos)
{
	DirectX::XMVECTOR cam_pos;
	cam_pos.m128_f32[0] = inCameraPos.x;
	cam_pos.m128_f32[1] = inCameraPos.y;
	cam_pos.m128_f32[2] = inCameraPos.z;
	cam_pos.m128_f32[3] = 1.0f;

	DirectX::XMMATRIX inverse_view = DirectX::XMMatrixInverse(nullptr, mCamera->GetViewMatrix());
	inverse_view = DirectX::XMMatrixTranspose(inverse_view);

	DirectX::XMVECTOR world_pos = DirectX::XMVector4Transform(cam_pos, inverse_view);
	float3 out_world_pos = float3(world_pos.m128_f32[0], world_pos.m128_f32[1], world_pos.m128_f32[2]);
	return out_world_pos;
}


float3 GaagFramework::ScreenToWorldPos(int2 inScreenPos)
{
	float3 cam_pos = ScreenToCameraPos(inScreenPos);
	float3 world_pos = CameraToWorldPos(cam_pos) - mCamera->GetPosition();
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
		theRenderContext.PSSetTextureAndSampler(diffuse_texture, mDefaultLinearSampler, 0);
	if (normal_texture != nullptr)
		theRenderContext.PSSetTextureAndSampler(normal_texture, mDefaultLinearSampler, 1);
	if (surface_texture != nullptr)
		theRenderContext.PSSetTextureAndSampler(surface_texture, mDefaultLinearSampler, 2);
	
	theRenderContext.PSSetConstantBuffer(inMaterial->GetConstantBuffer(), 0);
}


void GaagFramework::CopyToRenderTarget(pRenderTarget inTarget, pTexture inSource)
{
	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mCopyShader);
	theRenderContext.CSSetTexture(inSource, 0);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	int groups_x = (inTarget->GetTexture()->GetWidth() + 7) / 8;
	int groups_y = (inTarget->GetTexture()->GetHeight() + 7) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
}
