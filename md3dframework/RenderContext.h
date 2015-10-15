#pragma once
#include <d3d11_1.h>
#include <vector>
#include <atlbase.h>

#include "LinearAlgebra.h"


#ifndef D3DCall
#ifdef _DEBUG
#define D3DCall(x) \
{ \
	HRESULT hr = x; \
	if (FAILED(hr)) \
		{ \
		OutputDebugStringA(#x); \
		__debugbreak; \
		} \
}
#else
#define D3DCall(x) x
#endif
#endif


class PixelShader;
class VertexShader;
class ComputeShader;
class Texture;
class Sampler;
class Mesh;
class ConstantBuffer;
class RenderTarget;
class DepthStencilTarget;


#define theRenderContext RenderContext::GetInstance()


class RenderContext
{
public:
	static RenderContext& GetInstance()
	{
		static RenderContext instance;
		return instance;
	}

	ID3D11Device* GetDevice() { return mD3DDevice; }
	Texture* GetBackBuffer() { return mBackBuffer; }
	RenderTarget* GetOutputRenderTarget() { return mOutputRenderTarget; }

	HRESULT Init(HWND hWnd);
	bool IsInitialized() { return mInitialized; }
	void CleanUp();

	void BeginEvent(std::string inEventString) { mAnnotation->BeginEvent( std::wstring(inEventString.begin(), inEventString.end()).c_str() ); }
	void EndEvent() { mAnnotation->EndEvent(); }
	void SetMarker(std::string inMarker) { mAnnotation->SetMarker( std::wstring(inMarker.begin(), inMarker.end()).c_str() ); }

	void ClearDepthStencil(DepthStencilTarget* inDepthStencilTarget, float inClearDepth, unsigned char inClearStencil);
	void ClearRenderTarget(RenderTarget* inRenderTarget, float4 inColor);
	void SetRenderTargets(unsigned int inNumTargets, RenderTarget** inTargets, DepthStencilTarget* inDepthStencilTarget);
	void UpdateSubResource(ConstantBuffer* inConstantBuffer, const void* inData);
	void DrawMesh(Mesh* inMesh);
	void Flush();
	void SwapBuffers();

	void CSSetShader(ComputeShader* inComputeShader);
	void CSSetTexture(Texture* inTexture, int idx);
	void CSSetSampler(Sampler* inSampler, int idx);
	void CSSetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx);
	void CSSetConstantBuffer(ConstantBuffer* inConstantBuffer, int idx);
	void CSSetRWTexture(RenderTarget* inRenderTarget, int idx);
	void Dispatch(unsigned int inX, unsigned int inY, unsigned int inZ);
	
	void PSSetShader(PixelShader* inPixelShader);
	void PSSetTexture(Texture* inTexture, int idx);
	void PSSetSampler(Sampler* inSampler, int idx);
	void PSSetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx);
	void PSSetConstantBuffer(ConstantBuffer* inConstantBuffer, int idx);

	void VSSetShader(VertexShader* inVertexShader);
	void VSSetTexture(Texture* inTexture, int idx);
	void VSSetSampler(Sampler* inSampler, int idx);
	void VSSetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx);
	void VSSetConstantBuffer(ConstantBuffer* inConstantBuffer, int idx);

	unsigned int GetWidth() { return mWidth; }
	unsigned int GetHeight() { return mHeight; }

private:
	RenderContext() { mInitialized = false; }
	~RenderContext();
	RenderContext(RenderContext const&) = delete;
	void operator=(RenderContext const&) = delete;

	// for d3d interface
	D3D_DRIVER_TYPE						mDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL					mFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*						mD3DDevice = nullptr;
	ID3D11Device1*						mD3DDevice1 = nullptr;
	ID3D11DeviceContext*				mImmediateContext = nullptr;
	ID3D11DeviceContext1*				mImmediateContext1 = nullptr;
	IDXGISwapChain*						mSwapChain = nullptr;
	IDXGISwapChain1*					mSwapChain1 = nullptr;
	ID3D11RasterizerState*				mRasterState = nullptr;
	ID3D11RasterizerState1*				mRasterState1 = nullptr;
	ID3DUserDefinedAnnotation*			mAnnotation = nullptr;

	Texture*							mBackBuffer;
	RenderTarget*						mOutputRenderTarget;

	Texture*							mCSBoundTextures[8];
	Sampler*							mCSBoundSamplers[8];
	ConstantBuffer*						mCSBoundConstantBuffers[8];
	RenderTarget*						mCSBoundRenderTargets[8];
	
	Texture*							mPSBoundTextures[8];
	Sampler*							mPSBoundSamplers[8];
	ConstantBuffer*						mPSBoundConstantBuffers[8];

	Texture*							mVSBoundTextures[8];
	Sampler*							mVSBoundSamplers[8];
	ConstantBuffer*						mVSBoundConstantBuffers[8];
	unsigned int						mWidth, mHeight;
	bool								mInitialized = false;
};

