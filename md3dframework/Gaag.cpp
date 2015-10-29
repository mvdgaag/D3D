#include "Gaag.h"
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
	mWindow = std::make_shared<Window>();
	mWindow->Init(hInstance);
	theRenderContext.Init(mWindow);
	TextureUtil::InitTextureUtil();
	
	mDeferredRenderer = std::make_shared<DeferredRenderer>();
	mDeferredRenderer->Init(theRenderContext.GetWidth(), theRenderContext.GetHeight());

	mFullScreenTriangle = std::make_shared<Mesh>();
	mFullScreenTriangle->InitFullscreenTriangle();

	mCopyShader = std::make_shared<ComputeShader>();
	mCopyShader->InitFromFile("../md3dFramework/Shaders/CopyCompute.hlsl");

	mDefaultPointSampler = std::make_shared<Sampler>();
	mDefaultPointSampler->Init(0); // D3D11_FILTER_MIN_MAG_MIP_POINT

	mDefaultLinearSampler = std::make_shared<Sampler>();
	mDefaultLinearSampler->Init(21); // D3D11_FILTER_MIN_MAG_MIP_LINEAR

	mCamera = std::make_shared<Camera>();
	mCamera->SetPosition(0.0, 2.0, -3.0);
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

	int groups_x = 1 + (inTarget->GetTexture()->GetWidth() - 1) / 8;
	int groups_y = 1 + (inTarget->GetTexture()->GetHeight() - 1) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
}
