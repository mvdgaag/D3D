#include "Framework.h"
#include <d3d11_1.h>
#include <directxcolors.h>

#include "DeferredRenderer.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "GBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Texture.h"


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

	return hr;
}



float a = 0.666;
void Framework::Render()
{
	// setup camera
	mCamera->SetPosition(10.0 * sin(a), -5.0, 10.0 * cos(a));
	mCamera->SetTarget(0.0, 0.0, 0.0);
	mCamera->SetUp(0.0, 1.0, 0.0);
	a += 0.001;

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

	// set shaders and vertex latout
	ID3D11VertexShader* vs = mFullScreenQuadVertexShader->GetHandle();
	mImmediateContext->VSSetShader(vs, NULL, 0);
	mImmediateContext->IASetInputLayout(mFullScreenQuadVertexShader->GetLayout());
	ID3D11PixelShader* ps = mFullScreenQuadPixelShader->GetHandle();
	mImmediateContext->PSSetShader(ps, NULL, 0);

	// bind shader resources
	ID3D11ShaderResourceView* output_texture = mDeferredRenderer->GetGBuffer()->GetTexture(GBuffer::NORMAL)->GetShaderResourceView();
	output_texture = mDeferredRenderer->GetDirectLighting()->GetTexture()->GetShaderResourceView();
	
	mImmediateContext->PSSetShaderResources(0, 1, &output_texture);
	
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
}


void Framework::CleanUpDevice()
{
	mDeferredRenderer->CleanUp();
	delete mDeferredRenderer;
	delete mCamera;

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

}