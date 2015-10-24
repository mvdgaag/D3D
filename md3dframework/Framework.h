#pragma once
#include "LinearAlgebra.h"
#include <vector>
#include <Windows.h>

class DeferredRenderer;
class PixelShader;
class VertexShader;
class ComputeShader;
class Texture;
class Sampler;
class RenderTarget;
class Mesh;
class ConstantBuffer;
class DrawableObject;
class Camera;
class Material;


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
	void RegisterObject(DrawableObject* obj) { mObjectList.push_back(obj); }

	Camera*		GetCamera()														{ return mCamera; }
	int			GetFrameID()													{ return mFrameID; }
	double		GetFrameTime()													{ return mFrameTime; }
	double		GetFrameDeltaTime()												{ return mDeltaTime; }
	void		SetMaterial(Material* inMaterial);

	// helper functions
	Sampler*	GetPointSampler()												{ return mDefaultPointSampler; }
	Sampler*	GetLinearSampler()												{ return mDefaultLinearSampler; }
	void		CopyToRenderTarget(RenderTarget* inTarget, Texture* inSource);

private:
	Framework() { mInitialized = false; }
	~Framework();
	Framework(Framework const&) = delete;
	void operator=(Framework const&) = delete;

	Mesh*							mFullScreenTriangle = nullptr;
	ComputeShader*					mCopyShader = nullptr;
	Sampler*						mDefaultPointSampler = nullptr;
	Sampler*						mDefaultLinearSampler = nullptr;
	int								mFrameID;
	double							mFrameTime;
	double							mDeltaTime;
	Camera*							mCamera = nullptr;
	DeferredRenderer*				mDeferredRenderer = nullptr;
	std::vector<DrawableObject*>	mObjectList;
	bool							mInitialized = false;
	void							(*mFrameCallback)(void);
};

