#pragma once
#include "GaagCommon.h"

struct ID3D11Device;
struct ID3D11Device1;
struct ID3D11DeviceContext;
struct ID3D11DeviceContext1;
struct IDXGISwapChain;
struct IDXGISwapChain1;
struct ID3D11RasterizerState;
struct ID3D11RasterizerState1;
struct ID3DUserDefinedAnnotation;

PREDEFINE(Window, pWindow);
PREDEFINE(PixelShader, pPixelShader);
PREDEFINE(VertexShader, pVertexShader);
PREDEFINE(ComputeShader, pComputeShader);
PREDEFINE(Texture, pTexture);
PREDEFINE(Sampler, pSampler);
PREDEFINE(Mesh, pMesh);
PREDEFINE(ConstantBuffer, pConstantBuffer);
PREDEFINE(RenderTarget, pRenderTarget);
PREDEFINE(DepthStencilTarget, pDepthStencilTarget);

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
	pTexture		GetBackBuffer() const				{ return mBackBuffer; }
	pRenderTarget	GetOutputRenderTarget() const		{ return mOutputRenderTarget; }
	unsigned int	GetWidth() const					{ return mWidth; }
	unsigned int	GetHeight() const					{ return mHeight; }
	bool			IsInitialized() const				{ return mInitialized; }

	void Init(pWindow inWindow);
	void CleanUp();

	void BeginEvent(std::string inEventString);
	void EndEvent();
	void SetMarker(std::string inMarker);

	void ClearDepthStencil(pDepthStencilTarget inDepthStencilTarget, float inClearDepth, unsigned char inClearStencil);
	void ClearRenderTarget(pRenderTarget inRenderTarget, float4 inColor);
	void SetRenderTargets(unsigned int inNumTargets, pRenderTarget* inTargets, pDepthStencilTarget inDepthStencilTarget);
	void UpdateSubResource(const ConstantBuffer& inConstantBuffer, const void* inData);
	void CopySubResourceRegion(const Texture& dst, const Texture& src, const rect& inSourceRect, int2 inTargetCoord);
	void Map(const Texture& inTexture, void** outDataPtr, int inMapType = 1); // 1 == read
	void UnMap(const Texture& inTexture);
	void DrawMesh(const Mesh& inMesh);
	void Flush();
	void SwapBuffers();

	void CSSetShader(pComputeShader inComputeShader);
	
	// TODO: pass by reference for all other functions as well!
	void CSSetShader(const ComputeShader& inComputeShader);

	void CSSetTexture(pTexture inTexture, int idx);
	void CSSetSampler(pSampler inSampler, int idx);
	void CSSetTextureAndSampler(pTexture inTexture, pSampler inSampler, int idx);
	void CSSetConstantBuffer(pConstantBuffer inConstantBuffer, int idx);
	void CSSetRWTexture(pRenderTarget inRenderTarget, int idx);
	void Dispatch(unsigned int inX, unsigned int inY, unsigned int inZ);
	
	void PSSetShader(pPixelShader inPixelShader);
	void PSSetTexture(pTexture inTexture, int idx);
	void PSSetSampler(pSampler inSampler, int idx);
	void PSSetTextureAndSampler(pTexture inTexture, pSampler inSampler, int idx);
	void PSSetConstantBuffer(pConstantBuffer inConstantBuffer, int idx);

	void VSSetShader(pVertexShader inVertexShader);
	void VSSetTexture(pTexture inTexture, int idx);
	void VSSetSampler(pSampler inSampler, int idx);
	void VSSetTextureAndSampler(pTexture inTexture, pSampler inSampler, int idx);
	void VSSetConstantBuffer(pConstantBuffer inConstantBuffer, int idx);

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

	pTexture						mBackBuffer;
	pRenderTarget					mOutputRenderTarget;

	pTexture						mCSBoundTextures[8];
	pSampler						mCSBoundSamplers[8];
	pConstantBuffer					mCSBoundConstantBuffers[8];
	pRenderTarget					mCSBoundRenderTargets[8];

	pTexture						mPSBoundTextures[8];
	pSampler						mPSBoundSamplers[8];
	pConstantBuffer					mPSBoundConstantBuffers[8];

	pTexture						mVSBoundTextures[8];
	pSampler						mVSBoundSamplers[8];
	pConstantBuffer					mVSBoundConstantBuffers[8];
	unsigned int					mWidth, mHeight;
	bool							mInitialized = false;
};

