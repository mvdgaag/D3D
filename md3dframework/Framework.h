#pragma once
#include <d3d11_1.h>
#include "LinearAlgebra.h"
#include <vector>

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

	HRESULT InitDevice(HWND hWnd);
	HRESULT InitFullScreenQuad();
	void InitHelpers();
	void Render();
	void RegisterObject(DrawableObject* obj) { mObjectList.push_back(obj); }
	void CleanUpDevice();

	ID3D11Device* GetDevice() { return mD3DDevice; }
	Camera* GetCamera() { return mCamera; }
	int GetScreenWidth() { return mWidth; }
	int GetScreenHeight() { return mHeight; }
	int GetFrameID() { return mFrameID; }

	void SetMaterial(Material* inMaterial);

	void SetVertexShader(VertexShader* inVertexShader);
	void SetPixelShader(PixelShader* inPixelShader);
	void SetComputeShader(ComputeShader* inComputeShader);

	void SetTexture(Texture* inTexture, int idx);
	void SetSampler(Sampler* inSampler, int idx);
	void SetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx);
	void SetVertexConstantBuffer(ConstantBuffer* inConstantBuffer);
	void SetPixelConstantBuffer(ConstantBuffer* inConstantBuffer);

	Sampler* GetPointSampler() { return mDefaultPointSampler; }
	Sampler* GetLinearSampler() { return mDefaultLinearSampler; }

	void ComputeSetTexture(Texture* inTexture, int idx);
	void ComputeSetSampler(Sampler* inSampler, int idx);
	void ComputeSetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx);
	void ComputeSetVertexConstantBuffer(ConstantBuffer* inConstantBuffer);

	void ComputeSetRWTexture(RenderTarget* inRenderTarget, int idx);
	void ComputeDispatch(unsigned int inX, unsigned int inY, unsigned int inZ) { mImmediateContext->Dispatch(inX, inY, inZ); }
	void Flush() { mImmediateContext->Flush(); }

	void DrawMesh(Mesh* inMesh);

	void CopyToRenderTarget(RenderTarget* inTarget, Texture* inSource);

private:
	Framework() {}
	~Framework();
	Framework(Framework const&) = delete;
	void operator=(Framework const&) = delete;

	// for d3d interface
	D3D_DRIVER_TYPE					mDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL				mFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*					mD3DDevice = nullptr;
	ID3D11Device1*					mD3DDevice1 = nullptr;
	ID3D11DeviceContext*			mImmediateContext = nullptr;
	ID3D11DeviceContext1*			mImmediateContext1 = nullptr;
	IDXGISwapChain*					mSwapChain = nullptr;
	IDXGISwapChain1*				mSwapChain1 = nullptr;
	ID3D11RasterizerState*			mRasterState = nullptr;
	ID3D11RasterizerState1*			mRasterState1 = nullptr;
	ID3D11RenderTargetView*			mRenderTargetView = nullptr;

	// for rendering fullscreen quad
	ID3D11Buffer*					mFullScreenQuadVertexBuffer = nullptr;
	ID3D11Buffer*					mFullScreenQuadIndexBuffer = nullptr;
	VertexShader*					mFullScreenQuadVertexShader = nullptr;
	PixelShader*					mFullScreenQuadPixelShader = nullptr;
	ID3D11InputLayout*				mVertexLayout = nullptr;

	// helpers
	ComputeShader*					mCopyShader = nullptr;
	Sampler*						mDefaultPointSampler = nullptr;
	Sampler*						mDefaultLinearSampler = nullptr;

	// other globals
	int								mWidth, mHeight, mFrameID;
	Camera*							mCamera = nullptr;
	DeferredRenderer*				mDeferredRenderer = nullptr;
	std::vector<DrawableObject*>	mObjectList;
};

