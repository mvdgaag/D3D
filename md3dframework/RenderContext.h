#pragma once
#include <vector>
#include "LinearAlgebra.h"
#include <Windows.h>


struct ID3D11Device;
struct ID3D11Device1;
struct ID3D11DeviceContext;
struct ID3D11DeviceContext1;
struct IDXGISwapChain;
struct IDXGISwapChain1;
struct ID3D11RasterizerState;
struct ID3D11RasterizerState1;
struct ID3DUserDefinedAnnotation;


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
class Window;


#define theRenderContext RenderContext::GetInstance()


class RenderContext
{
public:
	static RenderContext& GetInstance()
	{
		static RenderContext instance;
		return instance;
	}

	// TODO: add create functions for shaders, buffers, etc, so no d3d link to this header is required anymore..
	ID3D11Device*	GetDevice() const					{ return mD3DDevice; }
	Texture*		GetBackBuffer() const				{ return mBackBuffer; }
	RenderTarget*	GetOutputRenderTarget() const		{ return mOutputRenderTarget; }
	unsigned int	GetWidth() const					{ return mWidth; }
	unsigned int	GetHeight() const					{ return mHeight; }
	bool			IsInitialized() const				{ return mInitialized; }

	HRESULT Init(Window* inWindow);
	void CleanUp();

	void BeginEvent(std::string inEventString);
	void EndEvent();
	void SetMarker(std::string inMarker);

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

private:
	RenderContext() { mInitialized = false; }
	~RenderContext();
	RenderContext(RenderContext const&) = delete;
	void operator=(RenderContext const&) = delete;

	// for d3d interface
	ID3D11Device*					mD3DDevice = nullptr;
	ID3D11Device1*					mD3DDevice1 = nullptr;
	ID3D11DeviceContext*			mImmediateContext = nullptr;
	ID3D11DeviceContext1*			mImmediateContext1 = nullptr;
	IDXGISwapChain*					mSwapChain = nullptr;
	IDXGISwapChain1*				mSwapChain1 = nullptr;
	ID3D11RasterizerState*			mRasterState = nullptr;
	ID3D11RasterizerState1*			mRasterState1 = nullptr;
	ID3DUserDefinedAnnotation*		mAnnotation = nullptr;

	Texture*						mBackBuffer;
	RenderTarget*					mOutputRenderTarget;

	Texture*						mCSBoundTextures[8];
	Sampler*						mCSBoundSamplers[8];
	ConstantBuffer*					mCSBoundConstantBuffers[8];
	RenderTarget*					mCSBoundRenderTargets[8];

	Texture*						mPSBoundTextures[8];
	Sampler*						mPSBoundSamplers[8];
	ConstantBuffer*					mPSBoundConstantBuffers[8];

	Texture*						mVSBoundTextures[8];
	Sampler*						mVSBoundSamplers[8];
	ConstantBuffer*					mVSBoundConstantBuffers[8];
	unsigned int					mWidth, mHeight;
	bool							mInitialized = false;
};

