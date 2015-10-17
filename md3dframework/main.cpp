//--------------------------------------------------------------------------------------
// File: Tutorial01.cpp
//
// This application demonstrates creating a Direct3D 11 device
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729718.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include <windows.h>
#include "resource.h"

#include "main.h"
#include "RenderContext.h"
#include "Framework.h"

// for content
#include "DrawableObject.h"
#include "TerrainTile.h"
#include "Mesh.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Texture.h"
#include "Sampler.h"
#include "Material.h"


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE						g_hInst = nullptr;
HWND							g_hWnd = nullptr;


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
void InitContent();
void CleanUpContent();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	if (FAILED(theRenderContext.Init(g_hWnd)))
	{
		theRenderContext.CleanUp();
		return 0;
	}

	theFramework.Init(g_hWnd);
	InitContent();

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			theFramework.Render();
		}
	}

	CleanUpContent();
	theFramework.CleanUp();
	theRenderContext.CleanUp();

	return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_MD3DFramework);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"D3DFramework";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"D3DFramework", L"D3DFramework",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// Note that this tutorial does not handle resizing (WM_SIZE) requests,
		// so we created the window without the resize border.

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


DrawableObject* g_obj;
Mesh* g_mesh;
PixelShader* g_pixel_shader;
VertexShader* g_vertex_shader;
Texture* g_diffuse_texture;
Texture* g_normal_texture;
Texture* g_surface_texture;
TerrainTile* g_terrainTile;
Material* g_material;


void InitContent()
{
	g_obj = new DrawableObject();
	
	g_mesh = new Mesh();
	g_mesh->InitFromFile("C:/Users/Maarten/Documents/Visual Studio 2013/Projects/D3D/C++/md3dframework/Models/sphere.obj");

	/*
	g_terrainTile = new TerrainTile();
	g_terrainTile->Init(float3(1, 1, 1), g_heightMap, 256, 256, g_pixel_shader, g_vertex_shader,
		g_diffuse_texture, g_diffuse_sampler, g_normal_texture, g_normal_sampler, g_surface_texture, g_surface_sampler);
	//g_mesh->InitPlane(4, 4, float2(4, 4));
	*/

	g_pixel_shader = new PixelShader();
	g_pixel_shader->InitFromFile("Shaders/BasicFragmentShader.hlsl");
	
	g_vertex_shader = new VertexShader();
	g_vertex_shader->InitFromFile("Shaders/BasicVertexShader.hlsl");

	g_diffuse_texture = new Texture();
	g_diffuse_texture->InitFromFile("Textures/photosculpt-squarebricks-diffuse.dds");

	g_normal_texture = new Texture();
	g_normal_texture->InitFromFile("Textures/photosculpt-squarebricks-normal.dds");

	g_surface_texture = new Texture();
	g_surface_texture->InitFromFile("Textures/photosculpt-squarebricks-specular.dds");

	g_material = new Material();
	g_material->Init();
	g_material->SetDiffuseTexture(g_diffuse_texture);
	g_material->SetNormalTexture(g_normal_texture);
	g_material->SetSurfaceTexture(g_surface_texture);
	g_material->SetDiffuseValue(float4(0.666, 0.4, 0, 0));
	g_material->SetReflectivityValue(0.5);
	g_material->SetRoughnessValue(0.5);
	g_material->SetMetalicityValue(0.0);
	g_material->SetEmissivenessValue(0.0);
	g_material->SetPixelShader(g_pixel_shader);
	g_material->SetVertexShader(g_vertex_shader);
	g_obj->Init(g_mesh, g_material);
	
	theFramework.RegisterObject(g_obj);
}


void CleanUpContent()
{
//	delete g_obj;
//	delete g_mesh;
//	delete g_pixel_shader;
//	delete g_vertex_shader;
//	delete g_diffuse_texture;
//	delete g_normal_texture;
//	delete g_surface_texture;
}