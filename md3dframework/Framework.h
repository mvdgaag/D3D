#pragma once
#include <d3d11_1.h>
#include "LinearAlgebra.h"
#include <vector>

class DeferredRenderer;
class PixelShader;
class VertexShader;
class Texture;
class Sampler;
class Mesh;
class ConstantBuffer;
class DrawableObject;
class Camera;


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
	void Render();
	void RegisterObject(DrawableObject* obj) { mObjectList.push_back(obj); }
	void CleanUpDevice();

	ID3D11Device* GetDevice() { return mD3DDevice; }
	Camera* GetCamera() { return mCamera; }
	int GetScreenWidth() { return mWidth; }
	int GetScreenHeight() { return mHeight; }

	void SetVertexShader(VertexShader* inVertexShader);
	void SetPixelShader(PixelShader* inPixelShader);
	void SetTexture(Texture* inTexture, int idx);
	void SetSampler(Sampler* inSampler, int idx);
	void SetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx);
	void SetConstantBuffer(ConstantBuffer* inConstantBuffer);
	void DrawMesh(Mesh* inMesh);

private:
	Framework() {}
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

	// other globals
	int								mWidth, mHeight;
	Camera*							mCamera = nullptr;
	DeferredRenderer*				mDeferredRenderer = nullptr;
	std::vector<DrawableObject*>	mObjectList;
};

