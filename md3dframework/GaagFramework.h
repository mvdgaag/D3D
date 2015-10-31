#pragma once
#include "GaagCommon.h"
#include "RenderContext.h"
#include "Input.h"
#include "Time.h"
#include "Window.h"
			
PREDEFINE(PixelShader, pPixelShader);
PREDEFINE(VertexShader, pVertexShader);
PREDEFINE(ComputeShader, pComputeShader);
PREDEFINE(Texture, pTexture);
PREDEFINE(Sampler, pSampler);
PREDEFINE(RenderTarget, pRenderTarget);
PREDEFINE(Mesh, pMesh);
PREDEFINE(ConstantBuffer, pConstantBuffer);
PREDEFINE(Material, pMaterial);
PREDEFINE(DeferredRenderer, pDeferredRenderer);
PREDEFINE(DrawableObject, pDrawableObject);
PREDEFINE(Camera, pCamera);

#define Gaag GaagFramework::GetInstance()


class GaagFramework
{
public:
	static GaagFramework& GetInstance()
	{
		static GaagFramework instance;
		return instance;
	}

	HRESULT Init(HINSTANCE hInstance);
	bool IsInitialized() { return mInitialized; }
	void CleanUp();

	void Render();
	void SetFrameCallback(void(*inCallBack)(void)) { mFrameCallback = inCallBack; }
	void RegisterObject(pDrawableObject obj) { mObjectList.push_back(obj); }

	pCamera		GetCamera()														{ return mCamera; }
	int			GetFrameID()													{ return mFrameID; }
	double		GetFrameTime()													{ return mFrameTime; }
	double		GetFrameDeltaTime()												{ return mDeltaTime; }
	void		SetMaterial(pMaterial inMaterial);

	// helper functions
	pSampler	GetPointSampler()												{ return mDefaultPointSampler; }
	pSampler	GetLinearSampler()												{ return mDefaultLinearSampler; }
	void		CopyToRenderTarget(pRenderTarget inTarget, pTexture inSource);

private:
	GaagFramework() { mInitialized = false; }
	~GaagFramework();
	GaagFramework(GaagFramework const&) = delete;
	void operator=(GaagFramework const&) = delete;

	pMesh							mFullScreenTriangle = nullptr;
	pComputeShader					mCopyShader = nullptr;
	pSampler						mDefaultPointSampler = nullptr;
	pSampler						mDefaultLinearSampler = nullptr;
	
	pWindow							mWindow;
	pCamera							mCamera = nullptr;
	pDeferredRenderer				mDeferredRenderer = nullptr;
	std::vector<pDrawableObject>	mObjectList;
	int								mFrameID;
	double							mFrameTime;
	double							mDeltaTime;
	bool							mInitialized = false;
	void							(*mFrameCallback)(void);
};

