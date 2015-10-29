#pragma once
#include "GAAGCommon.h"
#include <Windows.h>

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

#define theFramework Framework::GetInstance()


class Framework
{
public:
	static Framework& GetInstance()
	{
		static Framework instance;
		return instance;
	}

	HRESULT Init();
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
	Framework() { mInitialized = false; }
	~Framework();
	Framework(Framework const&) = delete;
	void operator=(Framework const&) = delete;

	pMesh							mFullScreenTriangle = nullptr;
	pComputeShader					mCopyShader = nullptr;
	pSampler						mDefaultPointSampler = nullptr;
	pSampler						mDefaultLinearSampler = nullptr;
	
	pCamera							mCamera = nullptr;
	pDeferredRenderer				mDeferredRenderer = nullptr;
	std::vector<pDrawableObject>	mObjectList;
	int								mFrameID;
	double							mFrameTime;
	double							mDeltaTime;
	bool							mInitialized = false;
	void							(*mFrameCallback)(void);
};

