#pragma once
#include "GaagCommon.h"

#include "RenderContext.h"
#include "ResourceFactory.h"
#include "DeferredRenderer.h"
#include "Input.h"
#include "Time.h"

#include "InputListener.h"
#include "Window.h"
			
REGISTERCLASS(DeferredRenderer);
REGISTERCLASS(MeshObject);
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
	
	void RegisterObject(pMeshObject inObject) { mObjectList.push_back(inObject); }

	void RegisterLight(pPointLight inLight) { mDeferredRenderer->RegisterLight(inLight); }
	void RegisterLight(pSpotLight inLight) { mDeferredRenderer->RegisterLight(inLight); }
	void RegisterLight(pDirectionalLight inLight) { mDeferredRenderer->RegisterLight(inLight); }

	float3 ScreenToCameraPos(int2 inScreenPos);
	float3 WorldToCameraPos(float3 inCameraPos);
	float3 CameraToWorldPos(float3 inCameraPos);
	float3 ScreenToWorldPos(int2 inScreenPos);
	float3 WorldToCameraNormal(float3 inCameraNormal);
	float3 CameraToWorldNormal(float3 inCameraNormal);

	pCamera		GetCamera()														{ return mCamera; }
	int			GetFrameID()													{ return mFrameID; }
	float		GetRandom()														{ return float(rand()) / RAND_MAX; }
	double		GetFrameTime()													{ return mFrameTime; }
	double		GetFrameDeltaTime()												{ return mDeltaTime; }
	void		SetMaterial(pMaterial inMaterial);
	void		CopyToRenderTarget(pRenderTarget inTarget, pTexture inSource, int inMipLevel);

private:
	GaagFramework() { mInitialized = false; }
	~GaagFramework();
	GaagFramework(GaagFramework const&) = delete;
	void operator=(GaagFramework const&) = delete;

	pMesh							mFullScreenTriangle = nullptr;
	pSampler						mDefaultSampler = nullptr;
	pComputeShader					mCopyShader = nullptr;
	pConstantBuffer					mCopyConstantBuffer = nullptr;
	pWindow							mWindow;
	pCamera							mCamera = nullptr;
	pDeferredRenderer				mDeferredRenderer = nullptr;
	std::vector<pMeshObject>	mObjectList;
	int								mFrameID;
	double							mFrameTime;
	double							mDeltaTime;
	bool							mInitialized = false;
	void							(*mFrameCallback)(void);
};

