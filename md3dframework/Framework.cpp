#include "Framework.h"
#include "RenderContext.h"
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


Framework::~Framework()
{
	CleanUp();
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT Framework::Init(HWND hWnd)
{
	assert(theRenderContext.IsInitialized());
	
	mDeferredRenderer = new DeferredRenderer();
	mDeferredRenderer->Init(theRenderContext.GetWidth(), theRenderContext.GetHeight());

	mFullScreenTriangle = new Mesh();
	mFullScreenTriangle->InitFullscreenTriangle();

	mCopyShader = new ComputeShader();
	mCopyShader->InitFromFile("Shaders/CopyCompute.hlsl");

	mDefaultPointSampler = new Sampler();
	mDefaultPointSampler->Init(0); // D3D11_FILTER_MIN_MAG_MIP_POINT

	mDefaultLinearSampler = new Sampler();
	mDefaultLinearSampler->Init(21); // D3D11_FILTER_MIN_MAG_MIP_LINEAR

	mCamera = new Camera();
	mFrameID = 0;

	mInitialized = true;
	return S_OK;
}


void Framework::CleanUp()
{
	delete mCamera;
	mCamera = nullptr;

	delete mDeferredRenderer;
	mDeferredRenderer = nullptr;
	
	delete mFullScreenTriangle;
	mFullScreenTriangle = nullptr;
	
	delete mCopyShader;
	mCopyShader = nullptr;
	
	delete mDefaultPointSampler;
	mDefaultPointSampler = nullptr;
	
	delete mDefaultLinearSampler;
	mDefaultLinearSampler = nullptr;
	
	mInitialized = false;
}


float a = 0.666;
void Framework::Render()
{
	assert(mInitialized);

	mCamera->SetPosition(3.0 * sin(a), -0.0, -5.0);
	mCamera->SetTarget(3.0 * sin(a), -0.0, 0.0);
	//mCamera->SetTarget(0.0, 0.0, 0.0);
	mCamera->SetUp(0.0, 1.0, 0.0);
	a -= 0.01;

	mDeferredRenderer->Render(mObjectList);

	CopyToRenderTarget(theRenderContext.GetOutputRenderTarget(), mDeferredRenderer->GetPostProcessed()->GetTexture());
	
	theRenderContext.SwapBuffers();

	mFrameID++;
}


void Framework::SetMaterial(Material* inMaterial)
{
	theRenderContext.PSSetShader(inMaterial->GetPixelShader());
	theRenderContext.VSSetShader(inMaterial->GetVertexShader());
	
	Texture* diffuse_texture = inMaterial->GetDiffuseTexture();
	Texture* normal_texture = inMaterial->GetNormalTexture();
	Texture* surface_texture = inMaterial->GetSurfaceTexture();
	
	if (diffuse_texture != nullptr)
		theRenderContext.PSSetTextureAndSampler(diffuse_texture, mDefaultLinearSampler, 0);
	if (normal_texture != nullptr)
		theRenderContext.PSSetTextureAndSampler(normal_texture, mDefaultLinearSampler, 1);
	if (surface_texture != nullptr)
		theRenderContext.PSSetTextureAndSampler(surface_texture, mDefaultLinearSampler, 2);
	
	theRenderContext.PSSetConstantBuffer(inMaterial->GetConstantBuffer(), 0);
}


void Framework::CopyToRenderTarget(RenderTarget* inTarget, Texture* inSource)
{
	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	theRenderContext.CSSetShader(mCopyShader);
	theRenderContext.CSSetTexture(inSource, 0);
	theRenderContext.CSSetRWTexture(inTarget, 0);

	int groups_x = 1 + (inTarget->GetTexture()->GetWidth() - 1) / 8;
	int groups_y = 1 + (inTarget->GetTexture()->GetHeight() - 1) / 8;
	theRenderContext.Dispatch(groups_x, groups_y, 1);

	// TODO: required?
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0);
}
