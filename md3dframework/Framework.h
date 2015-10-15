#pragma once
#include "RenderContext.h"
#include "LinearAlgebra.h"
#include <vector>

class RenderContext;
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

	HRESULT Init(HWND hWnd);
	bool IsInitialized() { return mInitialized; }
	void CleanUp();

	void Render();
	void RegisterObject(DrawableObject* obj) { mObjectList.push_back(obj); }

	Camera* GetCamera() { return mCamera; }
	int GetFrameID() { return mFrameID; }
	void SetMaterial(Material* inMaterial);
	Sampler* GetPointSampler() { return mDefaultPointSampler; }
	Sampler* GetLinearSampler() { return mDefaultLinearSampler; }
	void CopyToRenderTarget(RenderTarget* inTarget, Texture* inSource);

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
	Camera*							mCamera = nullptr;
	DeferredRenderer*				mDeferredRenderer = nullptr;
	std::vector<DrawableObject*>	mObjectList;
	bool							mInitialized = false;
};

