#include "RenderContext.h"
#include "Framework.h"
#include "Texture.h"
#include "Sampler.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "ComputeShader.h"
#include "ConstantBuffer.h"
#include "RenderTarget.h"
#include "DepthStencilTarget.h"
#include "Mesh.h"


RenderContext::~RenderContext()
{
	theFramework.CleanUp();
	CleanUp();
}


HRESULT RenderContext::Init(HWND hWnd)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hWnd, &rc);
	mWidth = rc.right - rc.left;
	mHeight = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		mDriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, mDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &mD3DDevice, &mFeatureLevel, &mImmediateContext);
		if (hr == E_INVALIDARG)
		{
			hr = D3D11CreateDevice(nullptr, mDriverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, &mD3DDevice, &mFeatureLevel, &mImmediateContext);
		}
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	IDXGIFactory1* dxgiFactory = nullptr;
	{
		IDXGIDevice* dxgiDevice = nullptr;
		hr = mD3DDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr))
		{
			IDXGIAdapter* adapter = nullptr;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
				adapter->Release();
			}
			dxgiDevice->Release();
		}
	}
	if (FAILED(hr))
		return hr;

	// Create swap chain
	IDXGIFactory2* dxgiFactory2 = nullptr;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
	if (dxgiFactory2)
	{
		// DirectX 11.1 or later
		hr = mD3DDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&mD3DDevice1));
		if (SUCCEEDED(hr))
		{
			(void)mImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&mImmediateContext1));
		}

		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Width = mWidth;
		sd.Height = mHeight;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS;
		sd.BufferCount = 1;

		hr = dxgiFactory2->CreateSwapChainForHwnd(mD3DDevice, hWnd, &sd, nullptr, nullptr, &mSwapChain1);
		if (SUCCEEDED(hr))
		{
			hr = mSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&mSwapChain));
		}

		dxgiFactory2->Release();
	}
	else
	{
		// DirectX 11.0 systems
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = mWidth;
		sd.BufferDesc.Height = mHeight;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		hr = dxgiFactory->CreateSwapChain(mD3DDevice, &sd, &mSwapChain);
	}

	// Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
	dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	dxgiFactory->Release();

	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;
	
	mBackBuffer = new Texture();
	mBackBuffer->Init(pBackBuffer);

	mOutputRenderTarget = new RenderTarget();
	mOutputRenderTarget->Init(mBackBuffer);

	// setup rasterizer
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	hr = mD3DDevice->CreateRasterizerState(&rasterDesc, &mRasterState);
	if (FAILED(hr))
		return hr;
	mImmediateContext->RSSetState(mRasterState);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)mWidth;
	vp.Height = (FLOAT)mHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mImmediateContext->RSSetViewports(1, &vp);

	hr = mImmediateContext->QueryInterface(__uuidof(mAnnotation), reinterpret_cast<void**>(&mAnnotation));
	if (FAILED(hr))
		return hr;

	mInitialized = true;
	return S_OK;
}


void RenderContext::CleanUp()
{
	if (mImmediateContext) mImmediateContext->ClearState();
	
	if (mSwapChain1) mSwapChain1->Release();
	if (mSwapChain) mSwapChain->Release();
	if (mRasterState) mRasterState->Release();
	if (mRasterState1) mRasterState1->Release();
	if (mImmediateContext1) mImmediateContext1->Release();
	if (mImmediateContext) mImmediateContext->Release();
	if (mD3DDevice1) mD3DDevice1->Release();
	if (mD3DDevice) mD3DDevice->Release();

	delete mOutputRenderTarget;
	delete mBackBuffer;

	mInitialized = false;
}


void RenderContext::ClearDepthStencil(DepthStencilTarget* inDepthStencilTarget, float inClearDepth, unsigned char inClearStencil)
{
	ID3D11DepthStencilView* dsv = inDepthStencilTarget->GetDepthStencilView();
	mImmediateContext->ClearDepthStencilView(dsv, 1, inClearDepth, inClearStencil); // TODO: now just depth (low bit)
}


void RenderContext::ClearRenderTarget(RenderTarget* inRenderTarget, float4 inColor)
{
	ID3D11RenderTargetView* rtv = inRenderTarget->GetRenderTargetView();
	mImmediateContext->ClearRenderTargetView(rtv, &inColor[0]);
}


#define MAX_RENDER_TARGETS (8)
void RenderContext::SetRenderTargets(unsigned int inNumTargets, RenderTarget** inTargets, DepthStencilTarget* inDepthStencilTarget)
{
	ID3D11RenderTargetView* targets[MAX_RENDER_TARGETS];
	for (int i = 0; i < inNumTargets; i++)
	{
		targets[i] = NULL;
		if (inTargets[i] != nullptr)
			targets[i] = inTargets[i]->GetRenderTargetView();
	}
	if (inDepthStencilTarget != nullptr)
		mImmediateContext->OMSetRenderTargets(inNumTargets, targets, inDepthStencilTarget->GetDepthStencilView());
	else
		mImmediateContext->OMSetRenderTargets(inNumTargets, targets, NULL);
}


void RenderContext::UpdateSubResource(ConstantBuffer* inConstantBuffer, const void* inData)
{
	mImmediateContext->UpdateSubresource(inConstantBuffer->GetBuffer(), 0, nullptr, &inData, 0, 0);
}


void RenderContext::DrawMesh(Mesh* inMesh)
{
	ID3D11Buffer* mesh_verts = inMesh->GetVertexBuffer();
	ID3D11Buffer* mesh_indices = inMesh->GetIndexBuffer();
	UINT stride = inMesh->GetStride();
	UINT offset = inMesh->GetOffset();
	mImmediateContext->IASetVertexBuffers(0, 1, &mesh_verts, &stride, &offset);
	mImmediateContext->IASetIndexBuffer(mesh_indices, DXGI_FORMAT_R16_UINT, 0);
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mImmediateContext->DrawIndexed(inMesh->GetNumIndices(), 0, 0);
}


void RenderContext::Flush()
{
	mImmediateContext->Flush();
}


void RenderContext::SwapBuffers()
{
	mSwapChain->Present(0, 0);
}


void RenderContext::CSSetShader(ComputeShader* inComputeShader)
{
	if (inComputeShader != nullptr)
		mImmediateContext->CSSetShader(inComputeShader->GetHandle(), NULL, 0);
	else
		mImmediateContext->CSSetShader(NULL, NULL, 0);
}


void RenderContext::CSSetTexture(Texture* inTexture, int idx)
{
	if (mCSBoundTextures[idx] == inTexture)
		return;
	else
	{
		ID3D11ShaderResourceView* srv = NULL;
		if (inTexture != nullptr)
			srv = inTexture->GetShaderResourceView();
		mImmediateContext->CSSetShaderResources(idx, 1, &srv);
		mCSBoundTextures[idx] = inTexture;
	}
}


void RenderContext::CSSetSampler(Sampler* inSampler, int idx)
{
	if (mCSBoundSamplers[idx] == inSampler)
		return;
	else
	{
		ID3D11SamplerState* samp = NULL;
		if (inSampler != nullptr)
			samp = inSampler->GetSamplerState();
		mImmediateContext->CSSetSamplers(idx, 1, &samp);
		mCSBoundSamplers[idx] = inSampler;
	}
}


void RenderContext::CSSetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx)
{
	CSSetTexture(inTexture, idx);
	CSSetSampler(inSampler, idx);
}


void RenderContext::CSSetConstantBuffer(ConstantBuffer* inConstantBuffer, int idx)
{
	if (mCSBoundConstantBuffers[idx] == inConstantBuffer)
		return;
	else
	{
		ID3D11Buffer* buffer = NULL;
		if (inConstantBuffer != nullptr)
			buffer = inConstantBuffer->GetBuffer();
		mImmediateContext->CSSetConstantBuffers(idx, 1, &buffer);
		mCSBoundConstantBuffers[idx] = inConstantBuffer;
	}
}


void RenderContext::CSSetRWTexture(RenderTarget* inRenderTarget, int idx)
{
	if (mCSBoundRenderTargets[idx] == inRenderTarget)
		return;
	else
	{
		ID3D11UnorderedAccessView* uav = NULL;
		if (inRenderTarget != nullptr)
			uav = inRenderTarget->GetUnorderedAccessView();
		mImmediateContext->CSSetUnorderedAccessViews(idx, 1, &uav, NULL);
		mCSBoundRenderTargets[idx] = inRenderTarget;
	}
}


void RenderContext::Dispatch(unsigned int inX, unsigned int inY, unsigned int inZ)
{ 
	mImmediateContext->Dispatch(inX, inY, inZ); 
}


void RenderContext::PSSetShader(PixelShader* inPixelShader)
{
	if (inPixelShader != nullptr)
		mImmediateContext->PSSetShader(inPixelShader->GetHandle(), NULL, 0);
	else
		mImmediateContext->PSSetShader(NULL, NULL, 0);
}


void RenderContext::PSSetTexture(Texture* inTexture, int idx)
{
	if (mPSBoundTextures[idx] == inTexture)
		return;
	else
	{
		ID3D11ShaderResourceView* srv = NULL;
		if (inTexture != nullptr)
			srv = inTexture->GetShaderResourceView();
		mImmediateContext->PSSetShaderResources(idx, 1, &srv);
		mPSBoundTextures[idx] = inTexture;
	}
}


void RenderContext::PSSetSampler(Sampler* inSampler, int idx)
{
	if (mPSBoundSamplers[idx] == inSampler)
		return;
	else
	{
		ID3D11SamplerState* samp = NULL;
		if (inSampler != nullptr)
			samp = inSampler->GetSamplerState();
		mImmediateContext->PSSetSamplers(idx, 1, &samp);
		mPSBoundSamplers[idx] = inSampler;
	}
}


void RenderContext::PSSetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx)
{
	PSSetTexture(inTexture, idx);
	PSSetSampler(inSampler, idx);
}


void RenderContext::PSSetConstantBuffer(ConstantBuffer* inConstantBuffer, int idx)
{
	if (mPSBoundConstantBuffers[idx] == inConstantBuffer)
		return;
	else
	{
		ID3D11Buffer* buffer = NULL;
		if (inConstantBuffer != nullptr)
			buffer = inConstantBuffer->GetBuffer();
		mImmediateContext->PSSetConstantBuffers(idx, 1, &buffer);
		mPSBoundConstantBuffers[idx] = inConstantBuffer;
	}
}


void RenderContext::VSSetShader(VertexShader* inVertexShader)
{
	if (inVertexShader != nullptr)
	{
		mImmediateContext->IASetInputLayout(inVertexShader->GetLayout());
		mImmediateContext->VSSetShader(inVertexShader->GetHandle(), NULL, 0);
	}
	else
		mImmediateContext->VSSetShader(NULL, NULL, 0);
}


void RenderContext::VSSetTexture(Texture* inTexture, int idx)
{
	if (mVSBoundTextures[idx] == inTexture)
		return;
	else
	{
		ID3D11ShaderResourceView* srv = NULL;
		if (inTexture != nullptr)
			srv = inTexture->GetShaderResourceView();
		mImmediateContext->VSSetShaderResources(idx, 1, &srv);
		mVSBoundTextures[idx] = inTexture;
	}
}


void RenderContext::VSSetSampler(Sampler* inSampler, int idx)
{
	if (mVSBoundSamplers[idx] == inSampler)
		return;
	else
	{
		ID3D11SamplerState* samp = NULL;
		if (inSampler != nullptr)
			samp = inSampler->GetSamplerState();
		mImmediateContext->VSSetSamplers(idx, 1, &samp);
		mVSBoundSamplers[idx] = inSampler;
	}
}


void RenderContext::VSSetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx)
{
	VSSetTexture(inTexture, idx);
	VSSetSampler(inSampler, idx);
}


void RenderContext::VSSetConstantBuffer(ConstantBuffer* inConstantBuffer, int idx)
{
	if (mVSBoundConstantBuffers[idx] == inConstantBuffer)
		return;
	else
	{
		ID3D11Buffer* buffer = NULL;
		if (inConstantBuffer != nullptr)
			buffer = inConstantBuffer->GetBuffer();
		mImmediateContext->VSSetConstantBuffers(idx, 1, &buffer);
		mVSBoundConstantBuffers[idx] = inConstantBuffer;
	}
}
