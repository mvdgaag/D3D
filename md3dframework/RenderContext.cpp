#include "RenderContext.h"
#include "GaagFramework.h"
#include "Texture.h"
#include "Sampler.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "ComputeShader.h"
#include "ConstantBuffer.h"
#include "RenderTarget.h"
#include "DepthStencilTarget.h"
#include "Mesh.h"
#include "Window.h"
#include <d3d11_1.h>


RenderContext::RenderContext()
{}


RenderContext::~RenderContext()
{
	Gaag.CleanUp();
	CleanUp();
}


void RenderContext::Init(const HWND inWindowHandle, const int inWidth, const int inHeight)
{
	CleanUp();

	DXGI_SWAP_CHAIN_DESC swapchain_desc;
	ZeroMemory(&swapchain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	mWidth = inWidth;
	mHeight = inHeight;
	mHWnd = inWindowHandle;

	swapchain_desc.BufferCount = 1;
	swapchain_desc.BufferDesc.Width = mWidth;
	swapchain_desc.BufferDesc.Height = mHeight;
	swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS;
	swapchain_desc.SampleDesc.Count = 1; // 4?
	swapchain_desc.SampleDesc.Quality = 0;
	swapchain_desc.Windowed = true;
	swapchain_desc.OutputWindow = inWindowHandle;
	swapchain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
		NULL, NULL, D3D11_SDK_VERSION,
		&swapchain_desc, &mSwapChain, &mD3DDevice,
		NULL, &mImmediateContext);
	assert(!FAILED(hr));

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	assert(!FAILED(hr));

	mBackBuffer = MAKE_NEW(Texture);
	mBackBuffer->Init(pBackBuffer);

	mOutputRenderTarget = MAKE_NEW(RenderTarget);
	mOutputRenderTarget->Init(mBackBuffer);

	// setup rasterizer
	SetRasterizerState(FILL_SOLID, CULL_BACK, true, 0, 0.0f, 0.0f, true, false, false, false);

	D3D11_DEPTH_STENCIL_DESC dsDesc;

	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState * pDSState;
	mD3DDevice->CreateDepthStencilState(&dsDesc, &pDSState);
	mImmediateContext->OMSetDepthStencilState(pDSState, 1);

	// Setup the viewport
	SetViewport(int2(mWidth, mHeight), 0.0f, 1.0f, int2(0, 0));

	hr = mImmediateContext->QueryInterface(__uuidof(mAnnotation), reinterpret_cast<void**>(&mAnnotation));
	assert(!FAILED(hr));

	mInitialized = true;
}


void RenderContext::CleanUp()
{
	if (mOutputRenderTarget)
	{
		delete mOutputRenderTarget;
		mOutputRenderTarget = nullptr;
	}

	if (mBackBuffer)
	{
		delete mBackBuffer;
		mBackBuffer = nullptr;
	}

	if (mImmediateContext) mImmediateContext->ClearState();

	if (mSwapChain) mSwapChain->Release();
	if (mRasterState) mRasterState->Release();
	if (mImmediateContext) mImmediateContext->Release();
	if (mD3DDevice) mD3DDevice->Release();

	mInitialized = false;
}


// TODO: Can we just resize the backbuffer without losing context?!
void RenderContext::Resize(int inWidth, int inHeight)
{
	if (mSwapChain)
	{
		mWidth = inWidth;
		mHeight = inHeight;

		SetRenderTargets(0, NULL, NULL);

		// Release all outstanding references to the swap chain's buffers.
		delete mOutputRenderTarget;
		delete mBackBuffer;

		// Preserve the existing buffer count and format.
		// Automatically choose the width and height to match the client rect for HWNDs.
		HRESULT hr;
		hr = mSwapChain->ResizeBuffers(0, mWidth, mHeight, DXGI_FORMAT_UNKNOWN, 0);
		assert(!FAILED(hr));

		// Get buffer and create a render-target-view.
		ID3D11Texture2D* pBuffer;
		ID3D11Texture2D* pBackBuffer = nullptr;
		hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
		
		mBackBuffer = MAKE_NEW(Texture);
		mBackBuffer->Init(pBackBuffer);

		mOutputRenderTarget = MAKE_NEW(RenderTarget);
		mOutputRenderTarget->Init(mBackBuffer);

		// Setup the viewport
		SetRenderTargets(1, &mOutputRenderTarget, NULL);
		SetViewport(int2(mWidth, mHeight), 0.0f, 1.0f, int2(0, 0));

		hr = mImmediateContext->QueryInterface(__uuidof(mAnnotation), reinterpret_cast<void**>(&mAnnotation));
		assert(!FAILED(hr));
	}
}


void RenderContext::BeginEvent(std::string inEventString)
{
	mAnnotation->BeginEvent(std::wstring(inEventString.begin(), inEventString.end()).c_str()); 
}


void RenderContext::EndEvent()
{
	mAnnotation->EndEvent(); 
}


void RenderContext::SetMarker(std::string inMarker)
{
	mAnnotation->SetMarker(std::wstring(inMarker.begin(), inMarker.end()).c_str()); 
}


void RenderContext::GenerateMips(pTexture inTexture)
{
	assert((inTexture->mBindFlags & (BindFlag::BIND_SHADER_RESOURCE | BindFlag::BIND_RENDER_TARGET)) == (BindFlag::BIND_SHADER_RESOURCE | BindFlag::BIND_RENDER_TARGET));
	assert(inTexture->mMiscFlags & MiscFlag::GENERATE_MIPS == MiscFlag::GENERATE_MIPS);

	mImmediateContext->GenerateMips(inTexture->mShaderResourceView);
}


void RenderContext::SetViewport(int2 inDimensions, float inMinDepth, float inMaxDepth, int2 inTopLeft)
{
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)inDimensions.x;
	vp.Height = (FLOAT)inDimensions.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mImmediateContext->RSSetViewports(1, &vp);
}


void RenderContext::SetRasterizerState(FillMode inFillMode, CullMode inCullMode, bool inFrontCounterClockwise,
	int inDepthBias, float inDepthBiasClamp, float inSlopeScaledDepthBias,
	bool inDepthClipEnable, bool inScissorEnable, bool inMultisampleEnable, bool inAntialiasedLineEnable)
{
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.FillMode = D3D11_FILL_MODE(inFillMode);
	rasterDesc.CullMode = D3D11_CULL_MODE(inCullMode);
	rasterDesc.FrontCounterClockwise = inFrontCounterClockwise;
	rasterDesc.DepthBias = inDepthBias;
	rasterDesc.DepthBiasClamp = inDepthBiasClamp;
	rasterDesc.SlopeScaledDepthBias = inSlopeScaledDepthBias;
	rasterDesc.DepthClipEnable = inDepthClipEnable;
	rasterDesc.ScissorEnable = inScissorEnable;
	rasterDesc.MultisampleEnable = inMultisampleEnable;
	rasterDesc.AntialiasedLineEnable = inAntialiasedLineEnable;

	HRESULT hr = mD3DDevice->CreateRasterizerState(&rasterDesc, &mRasterState);
	assert(!FAILED(hr));
	mImmediateContext->RSSetState(mRasterState);
}


void RenderContext::ClearDepthStencil(pDepthStencilTarget inDepthStencilTarget, float inClearDepth, unsigned char inClearStencil)
{
	ID3D11DepthStencilView* dsv = inDepthStencilTarget->mDepthStencilView;
	mImmediateContext->ClearDepthStencilView(dsv, 1, inClearDepth, inClearStencil); // TODO: now just depth (low bit)
}


void RenderContext::ClearRenderTarget(pRenderTarget inRenderTarget, float4 inColor)
{
	for (int i = 0; i < inRenderTarget->mTexture->GetMipLevels(); i++)
	{
		ID3D11RenderTargetView* rtv = inRenderTarget->mRenderTargetViews[i];
		mImmediateContext->ClearRenderTargetView(rtv, &inColor[0]);
	}
}


#define MAX_RENDER_TARGETS (8)
void RenderContext::SetRenderTargets(unsigned int inNumTargets, pRenderTarget* inTargets, pDepthStencilTarget inDepthStencilTarget)
{
	ID3D11RenderTargetView* targets[MAX_RENDER_TARGETS];
	for (int i = 0; i < inNumTargets; i++)
	{
		targets[i] = NULL;
		if (inTargets[i] != nullptr)
			targets[i] = inTargets[i]->mRenderTargetViews[0];
	}
	if (inDepthStencilTarget != nullptr)
		mImmediateContext->OMSetRenderTargets(inNumTargets, targets, inDepthStencilTarget->mDepthStencilView);
	else
		mImmediateContext->OMSetRenderTargets(inNumTargets, targets, NULL);
}


void RenderContext::UpdateSubResource(const ConstantBuffer& inConstantBuffer, const void* inData)
{
	mImmediateContext->UpdateSubresource(inConstantBuffer.mBuffer, 0, nullptr, inData, 0, 0);
}


void RenderContext::CopySubResourceRegion(const Texture& dst, const Texture& src, const rect& inSourceRect, int2 inTargetCoord)
{
	D3D11_BOX srcBox;
	srcBox.left = inSourceRect.topLeft.x;
	srcBox.right = inSourceRect.bottomRight.x;
	srcBox.top = inSourceRect.topLeft.y;
	srcBox.bottom = inSourceRect.bottomRight.y;
	srcBox.front = 0;
	srcBox.back = 1;
	mImmediateContext->CopySubresourceRegion(dst.mTexture, 0, inTargetCoord.x, inTargetCoord.y, 0, src.mTexture, 0, &srcBox);
}


void RenderContext::Map(const Texture& inTexture, void** outDataPtr, int inMapType)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	mImmediateContext->Map(inTexture.mTexture, 0, (D3D11_MAP)inMapType, 0, &msr);
	outDataPtr[0] = msr.pData;
}


void RenderContext::UnMap(const Texture& inTexture)
{
	mImmediateContext->Unmap(inTexture.mTexture, 0);
}


void RenderContext::DrawMesh(const Mesh& inMesh)
{
	ID3D11Buffer* mesh_verts = inMesh.mVertexBuffer;
	ID3D11Buffer* mesh_indices = inMesh.mIndexBuffer;
	UINT stride = inMesh.mStride;
	UINT offset = inMesh.mOffset;
	mImmediateContext->IASetVertexBuffers(0, 1, &mesh_verts, &stride, &offset);
	mImmediateContext->IASetIndexBuffer(mesh_indices, DXGI_FORMAT_R16_UINT, 0);
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mImmediateContext->DrawIndexed(inMesh.mNumIndices, 0, 0);
}


void RenderContext::DrawShadowMesh(const Mesh& inMesh)
{
	// TODO: just positions!
	// Make positions non-interleaved?

	ID3D11Buffer* mesh_verts = inMesh.mVertexBuffer;
	ID3D11Buffer* mesh_indices = inMesh.mIndexBuffer;
	UINT stride = inMesh.mStride;
	UINT offset = inMesh.mOffset;
	mImmediateContext->IASetVertexBuffers(0, 1, &mesh_verts, &stride, &offset);
	mImmediateContext->IASetIndexBuffer(mesh_indices, DXGI_FORMAT_R16_UINT, 0);
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mImmediateContext->DrawIndexed(inMesh.mNumIndices, 0, 0);
}


void RenderContext::Flush()
{
	mImmediateContext->Flush();
}


void RenderContext::SwapBuffers()
{
	mSwapChain->Present(0, 0);
}


void RenderContext::CSSetShader(pComputeShader inComputeShader)
{
	if (inComputeShader != nullptr)
		mImmediateContext->CSSetShader(inComputeShader->mHandle, NULL, 0);
	else
		mImmediateContext->CSSetShader(NULL, NULL, 0);
}


void RenderContext::CSSetShader(const ComputeShader& inComputeShader)
{
	mImmediateContext->CSSetShader(inComputeShader.mHandle, NULL, 0);
}


void RenderContext::CSSetTexture(pTexture inTexture, int idx)
{
	if (mCSBoundTextures[idx] == inTexture)
		return;
	else
	{
		ID3D11ShaderResourceView* srv = NULL;
		if (inTexture != nullptr)
			srv = inTexture->mShaderResourceView;
		mImmediateContext->CSSetShaderResources(idx, 1, &srv);
		mCSBoundTextures[idx] = inTexture;
	}
}


void RenderContext::CSSetTextureSingleMip(pTexture inTexture, int inMipLevel, int idx)
{
	assert(inTexture != nullptr);
	assert(inTexture->mMipLevels > inMipLevel);
	assert(inTexture->mMipShaderResourceViews[inMipLevel] != nullptr);
	ID3D11ShaderResourceView* srv = inTexture->mMipShaderResourceViews[inMipLevel];
	mImmediateContext->CSSetShaderResources(idx, 1, &srv);
	mCSBoundTextures[idx] = nullptr; // single mip texture doesn't count and will have to be rebound
}


void RenderContext::CSSetSampler(pSampler inSampler, int idx)
{
	if (mCSBoundSamplers[idx] == inSampler)
		return;
	else
	{
		ID3D11SamplerState* samp = NULL;
		if (inSampler != nullptr)
			samp = inSampler->mSamplerState;
		mImmediateContext->CSSetSamplers(idx, 1, &samp);
		mCSBoundSamplers[idx] = inSampler;
	}
}


void RenderContext::CSSetTextureAndSampler(pTexture inTexture, pSampler inSampler, int idx)
{
	CSSetTexture(inTexture, idx);
	CSSetSampler(inSampler, idx);
}


void RenderContext::CSSetConstantBuffer(pConstantBuffer inConstantBuffer, int idx)
{
	if (mCSBoundConstantBuffers[idx] == inConstantBuffer)
		return;
	else
	{
		ID3D11Buffer* buffer = NULL;
		if (inConstantBuffer != nullptr)
			buffer = inConstantBuffer->mBuffer;
		mImmediateContext->CSSetConstantBuffers(idx, 1, &buffer);
		mCSBoundConstantBuffers[idx] = inConstantBuffer;
	}
}


void RenderContext::CSSetRWTexture(pRenderTarget inRenderTarget, int inMipLevel, int idx)
{
	ID3D11UnorderedAccessView* uav = NULL;
	if (inRenderTarget != nullptr)
		uav = inRenderTarget->mUnorderedAccessViews[inMipLevel];
	const unsigned int initial_count = -1;
	mImmediateContext->CSSetUnorderedAccessViews(idx, 1, &uav, &initial_count);
	mCSBoundRenderTargets[idx] = inRenderTarget;
}


void RenderContext::Dispatch(unsigned int inX, unsigned int inY, unsigned int inZ)
{ 
	mImmediateContext->Dispatch(inX, inY, inZ); 
}


void RenderContext::PSSetShader(pPixelShader inPixelShader)
{
	if (inPixelShader != nullptr)
		mImmediateContext->PSSetShader(inPixelShader->mHandle, NULL, 0);
	else
		mImmediateContext->PSSetShader(NULL, NULL, 0);
}


void RenderContext::PSSetTexture(pTexture inTexture, int idx)
{
	if (mPSBoundTextures[idx] == inTexture)
		return;
	else
	{
		ID3D11ShaderResourceView* srv = NULL;
		if (inTexture != nullptr)
			srv = inTexture->mShaderResourceView;
		mImmediateContext->PSSetShaderResources(idx, 1, &srv);
		mPSBoundTextures[idx] = inTexture;
	}
}


void RenderContext::PSSetSampler(pSampler inSampler, int idx)
{
	if (mPSBoundSamplers[idx] == inSampler)
		return;
	else
	{
		ID3D11SamplerState* samp = NULL;
		if (inSampler != nullptr)
			samp = inSampler->mSamplerState;
		mImmediateContext->PSSetSamplers(idx, 1, &samp);
		mPSBoundSamplers[idx] = inSampler;
	}
}


void RenderContext::PSSetTextureAndSampler(pTexture inTexture, pSampler inSampler, int idx)
{
	PSSetTexture(inTexture, idx);
	PSSetSampler(inSampler, idx);
}


void RenderContext::PSSetConstantBuffer(pConstantBuffer inConstantBuffer, int idx)
{
	if (mPSBoundConstantBuffers[idx] == inConstantBuffer)
		return;
	else
	{
		ID3D11Buffer* buffer = NULL;
		if (inConstantBuffer != nullptr)
			buffer = inConstantBuffer->mBuffer;
		mImmediateContext->PSSetConstantBuffers(idx, 1, &buffer);
		mPSBoundConstantBuffers[idx] = inConstantBuffer;
	}
}


void RenderContext::VSSetShader(pVertexShader inVertexShader)
{
	if (inVertexShader != nullptr)
	{
		mImmediateContext->IASetInputLayout(inVertexShader->mVertexLayout);
		mImmediateContext->VSSetShader(inVertexShader->mHandle, NULL, 0);
	}
	else
		mImmediateContext->VSSetShader(NULL, NULL, 0);
}


void RenderContext::VSSetTexture(pTexture inTexture, int idx)
{
	if (mVSBoundTextures[idx] == inTexture)
		return;
	else
	{
		ID3D11ShaderResourceView* srv = NULL;
		if (inTexture != nullptr)
			srv = inTexture->mShaderResourceView;
		mImmediateContext->VSSetShaderResources(idx, 1, &srv);
		mVSBoundTextures[idx] = inTexture;
	}
}


void RenderContext::VSSetSampler(pSampler inSampler, int idx)
{
	if (mVSBoundSamplers[idx] == inSampler)
		return;
	else
	{
		ID3D11SamplerState* samp = NULL;
		if (inSampler != nullptr)
			samp = inSampler->mSamplerState;
		mImmediateContext->VSSetSamplers(idx, 1, &samp);
		mVSBoundSamplers[idx] = inSampler;
	}
}


void RenderContext::VSSetTextureAndSampler(pTexture inTexture, pSampler inSampler, int idx)
{
	VSSetTexture(inTexture, idx);
	VSSetSampler(inSampler, idx);
}


void RenderContext::VSSetConstantBuffer(pConstantBuffer inConstantBuffer, int idx)
{
	if (mVSBoundConstantBuffers[idx] == inConstantBuffer)
		return;
	else
	{
		ID3D11Buffer* buffer = NULL;
		if (inConstantBuffer != nullptr)
			buffer = inConstantBuffer->mBuffer;
		mImmediateContext->VSSetConstantBuffers(idx, 1, &buffer);
		mVSBoundConstantBuffers[idx] = inConstantBuffer;
	}
}


