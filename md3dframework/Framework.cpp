#include "Framework.h"
#include <d3d11_1.h>
#include <directxcolors.h>

#include "DeferredRenderer.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "GBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "Sampler.h"
#include "ConstantBuffer.h"
#include "Mesh.h"



Framework::~Framework()
{
	mDeferredRenderer->CleanUp();
	delete mDeferredRenderer;
	delete mCamera;
	delete mCopyShader;
	if (mFullScreenQuadVertexBuffer) 
		mFullScreenQuadVertexBuffer->Release();
	if (mFullScreenQuadIndexBuffer)
		mFullScreenQuadIndexBuffer->Release();
	if (mVertexLayout)
		mVertexLayout->Release();
	delete mFullScreenQuadVertexShader;
	delete mFullScreenQuadPixelShader;
	delete mDefaultLinearSampler;
	delete mDefaultPointSampler;
	CleanUpDevice();
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT Framework::InitDevice(HWND hWnd)
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
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
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
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
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
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
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

	hr = mD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	mDeferredRenderer = new DeferredRenderer();
	mDeferredRenderer->Init(mWidth, mHeight);

	mCamera = new Camera();

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

	mFrameID = 0;

	return S_OK;
}


HRESULT Framework::InitFullScreenQuad()
{
	HRESULT hr = S_OK;

	mFullScreenQuadPixelShader = new PixelShader("FullScreenQuadPixelShader");
	mFullScreenQuadPixelShader->InitFromFile("Shaders/FullScreenQuadPixelShader.hlsl");

	mFullScreenQuadVertexShader = new VertexShader("FullScreenQuadVertexShader");
	mFullScreenQuadVertexShader->InitFromFile("Shaders/FullScreenQuadVertexShader.hlsl");

	float quad_vertices[] =
	{
		-1.0f, -1.0f, 0.5f,
		1.0f, -1.0f, 0.5f,
		1.0f, 1.0f, 0.5f,
		-1.0f, 1.0f, 0.5f,
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(float) * 3 * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = quad_vertices;
	hr = mD3DDevice->CreateBuffer(&bd, &InitData, &mFullScreenQuadVertexBuffer);
	if (FAILED(hr))
		return hr;

	WORD quad_indices[] =
	{
		3, 1, 0,
		2, 1, 3,
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = quad_indices;
	hr = mD3DDevice->CreateBuffer(&bd, &InitData, &mFullScreenQuadIndexBuffer);

	mDefaultPointSampler = new Sampler("DefaultPointSampler");
	mDefaultPointSampler->Init(D3D11_FILTER_MIN_MAG_MIP_POINT);

	mDefaultLinearSampler = new Sampler("DefaultLinearSampler");
	mDefaultLinearSampler->Init(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	return hr;
}


void Framework::InitHelpers()
{
	mCopyShader = new ComputeShader("CopyShader");
	mCopyShader->InitFromFile("Shaders/CopyCompute.hlsl");
}



float a = 0.666;
void Framework::Render()
{
	// setup camera
	mCamera->SetPosition(5.0 * sin(a), -3.0, 5.0 * cos(a));
	mCamera->SetTarget(0.0, 0.0, 0.0);
	mCamera->SetUp(0.0, 1.0, 0.0);
	a += 0.01;

	// render to gbuffer
	mDeferredRenderer->Render(mObjectList);

	// render to screen
	mImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
	mImmediateContext->ClearRenderTargetView(mRenderTargetView, DirectX::Colors::MidnightBlue);

	// set vertex buffer for full screen quad
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	mImmediateContext->IASetVertexBuffers(0, 1, &mFullScreenQuadVertexBuffer, &stride, &offset);
	mImmediateContext->IASetIndexBuffer(mFullScreenQuadIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SetVertexShader(mFullScreenQuadVertexShader);
	SetPixelShader(mFullScreenQuadPixelShader);

	SetTextureAndSampler(mDeferredRenderer->GetAntiAliased()->GetTexture(), mDefaultPointSampler, 0);

	// DEVHACK
	SetTextureAndSampler(mDeferredRenderer->GetDirectLighting()->GetTexture(), mDefaultPointSampler, 0);
	
	// draw
	mImmediateContext->DrawIndexed(6, 0, 0);

	// reset state
	ID3D11ShaderResourceView* none[] = { NULL };
	mImmediateContext->PSSetShaderResources(0, 1, none);
	ID3D11RenderTargetView* render_target = NULL;
	mImmediateContext->OMSetRenderTargets(1, &render_target, NULL);
	mImmediateContext->PSSetShader(NULL, NULL, 0);
	mImmediateContext->VSSetShader(NULL, NULL, 0);

	// swap
	mSwapChain->Present(0, 0);

	mFrameID++;
}


void Framework::CleanUpDevice()
{
	if (mImmediateContext) mImmediateContext->ClearState();
	if (mRenderTargetView) mRenderTargetView->Release();
	if (mSwapChain1) mSwapChain1->Release();
	if (mSwapChain) mSwapChain->Release();
	if (mRasterState) mRasterState->Release();
	if (mRasterState1) mRasterState1->Release();
	if (mImmediateContext1) mImmediateContext1->Release();
	if (mImmediateContext) mImmediateContext->Release();
	if (mD3DDevice1) mD3DDevice1->Release();
	if (mD3DDevice) mD3DDevice->Release();
}


void Framework::SetVertexShader(VertexShader* inVertexShader)
{
	ID3D11VertexShader* vs = NULL;
	if (inVertexShader)
	{
		vs = inVertexShader->GetHandle();
		mImmediateContext->IASetInputLayout(inVertexShader->GetLayout());
	}
	mImmediateContext->VSSetShader(vs, NULL, 0);
}


void Framework::SetPixelShader(PixelShader* inPixelShader)
{
	ID3D11PixelShader* ps = NULL;
	if (inPixelShader)
		ps = inPixelShader->GetHandle();
	mImmediateContext->PSSetShader(ps, NULL, 0);
}


void Framework::SetComputeShader(ComputeShader* inComputeShader)
{
	ID3D11ComputeShader* cs = NULL;
	if (inComputeShader)
		cs = inComputeShader->GetHandle();
	mImmediateContext->CSSetShader(cs, NULL, 0);
}


void Framework::SetTexture(Texture* inTexture, int idx)
{
	ID3D11ShaderResourceView* tex = NULL;
	if (inTexture)
		tex = inTexture->GetShaderResourceView();
	mImmediateContext->PSSetShaderResources(idx, 1, &tex);
}


void Framework::SetSampler(Sampler* inSampler, int idx)
{
	ID3D11SamplerState* smp = NULL;
	if (inSampler)
		smp = inSampler->GetSamplerState();
	mImmediateContext->PSSetSamplers(idx, 1, &smp);
}


void Framework::SetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx)
{
	SetTexture(inTexture, idx);
	SetSampler(inSampler, idx);
}


void Framework::SetConstantBuffer(ConstantBuffer* inConstantBuffer)
{
	ID3D11Buffer* cbuf = inConstantBuffer->GetBuffer();
	mImmediateContext->VSSetConstantBuffers(0, 1, &cbuf);
}


void Framework::ComputeSetTexture(Texture* inTexture, int idx)
{
	ID3D11ShaderResourceView* tex = NULL;
	if (inTexture)
		tex = inTexture->GetShaderResourceView();
	mImmediateContext->CSSetShaderResources(idx, 1, &tex);
}


void Framework::ComputeSetSampler(Sampler* inSampler, int idx)
{
	ID3D11SamplerState* smp = NULL;
	if (inSampler)
		smp = inSampler->GetSamplerState();
	mImmediateContext->CSGetSamplers(idx, 1, &smp);
}


void Framework::ComputeSetTextureAndSampler(Texture* inTexture, Sampler* inSampler, int idx)
{
	ComputeSetTexture(inTexture, idx);
	ComputeSetSampler(inSampler, idx);
}


void Framework::ComputeSetRWTexture(RenderTarget* inRenderTarget, int idx)
{
	ID3D11UnorderedAccessView* target = NULL;
	if (inRenderTarget)
		target = inRenderTarget->GetUnorderedAccessView();
	unsigned int initial_counts[] = { -1 };
	mImmediateContext->CSSetUnorderedAccessViews(idx, 1, &target, initial_counts);
}


void Framework::DrawMesh(Mesh* inMesh)
{
	ID3D11Buffer* mesh_verts = inMesh->GetVertexBuffer();
	UINT stride = inMesh->GetStride();
	UINT offset = inMesh->GetOffset();
	mImmediateContext->IASetVertexBuffers(0, 1, &mesh_verts, &stride, &offset);

	ID3D11Buffer* mesh_indices = inMesh->GetIndexBuffer();
	mImmediateContext->IASetIndexBuffer(mesh_indices, DXGI_FORMAT_R16_UINT, 0);

	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mImmediateContext->DrawIndexed(inMesh->GetNumIndices(), 0, 0);
}


void Framework::CopyToRenderTarget(RenderTarget* inTarget, Texture* inSource)
{
	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	SetComputeShader(mCopyShader);
	ComputeSetTexture(inSource, 0);
	ComputeSetRWTexture(inTarget, 0);

	int groups_x = 1 + (inTarget->GetTexture()->GetWidth() - 1) / 8;
	int groups_y = 1 + (inTarget->GetTexture()->GetHeight() - 1) / 8;
	ComputeDispatch(groups_x, groups_y, 1);

	// TODO: required?
	Flush();

	// clear state
	theFramework.SetComputeShader(NULL);
	theFramework.ComputeSetTexture(NULL, 0);
	theFramework.ComputeSetRWTexture(NULL, 0);
}