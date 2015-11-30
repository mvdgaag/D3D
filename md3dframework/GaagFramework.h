#pragma once
#include "GaagCommon.h"

#include "RenderContext.h"
#include "ResourceFactory.h"
#include "Input.h"
#include "Time.h"

#include "InputListener.h"
#include "Window.h"
			
REGISTERCLASS(PixelShader);
REGISTERCLASS(VertexShader);
REGISTERCLASS(ComputeShader);
REGISTERCLASS(Texture);
REGISTERCLASS(Sampler);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(Mesh);
REGISTERCLASS(ConstantBuffer);
REGISTERCLASS(Material);
REGISTERCLASS(DeferredRenderer);
REGISTERCLASS(DrawableObject);
REGISTERCLASS(Camera);

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
	float3 ScreenToCameraPos(int2 inScreenPos);
	float3 CameraToWorldPos(float3 inCameraPos);
	float3 ScreenToWorldPos(int2 inScreenPos);

	pCamera		GetCamera()														{ return mCamera; }
	int			GetFrameID()													{ return mFrameID; }
	double		GetFrameTime()													{ return mFrameTime; }
	double		GetFrameDeltaTime()												{ return mDeltaTime; }
	void		SetMaterial(pMaterial inMaterial);
	void		CopyToRenderTarget(pRenderTarget inTarget, pTexture inSource);

private:
	GaagFramework() { mInitialized = false; }
	~GaagFramework();
	GaagFramework(GaagFramework const&) = delete;
	void operator=(GaagFramework const&) = delete;

	pMesh							mFullScreenTriangle = nullptr;
	pSampler						mDefaultSampler = nullptr;
	pComputeShader					mCopyShader = nullptr;
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

