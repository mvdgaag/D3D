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
#include <windowsx.h>
#include "resource.h"

#include "main.h"
#include "RenderContext.h"
#include "Framework.h"
#include "Input.h"

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



DrawableObject* g_obj;
DrawableObject* g_obj2;

Mesh* g_mesh;
Mesh* g_mesh2;

Material* g_material;
Material* g_material2;

PixelShader* g_pixel_shader;
VertexShader* g_vertex_shader;
Texture* g_diffuse_texture;
Texture* g_normal_texture;
Texture* g_surface_texture;

Texture* g_heightmap;
TerrainTile* g_terrainTile;


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
			g_obj->Rotate(float3(0, 1, 0), 0.01);
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

	case WM_KEYDOWN:
		theInput.OnKeyDown(wParam);
		break;

	case WM_KEYUP:
		theInput.OnKeyUp(wParam);
		break;

	case WM_MOUSEMOVE:
		theInput.OnMouseMove(float2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;

	case WM_LBUTTONDOWN:
		theInput.OnMouseDown(0);
		break;

	case WM_LBUTTONUP:
		theInput.OnMouseUp(0);
		break;

	case WM_RBUTTONDOWN:
		theInput.OnMouseDown(1);
		break;

	case WM_RBUTTONUP:
		theInput.OnMouseUp(1);
		break;

	case WM_MBUTTONDOWN:
		theInput.OnMouseDown(2);
		break;

	case WM_MBUTTONUP:
		theInput.OnMouseUp(2);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


void InitContent()
{
	g_obj = new DrawableObject();
	g_obj2 = new DrawableObject();

	g_mesh = new Mesh();
	g_mesh->InitFromFile("C:/Users/Maarten/Documents/Visual Studio 2013/Projects/D3D/C++/md3dframework/Models/sphere.obj");

	g_mesh2 = new Mesh();
	g_mesh2->InitPlane(1, 1, float2(8, 8));

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
	g_material->SetDiffuseValue(float4(0.6, 0.4, 0.0, 0.0));
	g_material->SetReflectivityValue(0.1);
	g_material->SetRoughnessValue(0.9);
	g_material->SetMetalicityValue(0.0);
	g_material->SetEmissivenessValue(0.0);
	g_material->SetPixelShader(g_pixel_shader);
	g_material->SetVertexShader(g_vertex_shader);
	//g_material->SetFlags(Material::MaterialFlags(3));
	g_obj->Init(g_mesh, g_material);

	g_material2 = new Material();
	g_material2->Init();
	g_material2->SetDiffuseTexture(g_diffuse_texture);
	g_material2->SetNormalTexture(g_normal_texture);
	g_material2->SetSurfaceTexture(g_surface_texture);
	g_material2->SetDiffuseValue(float4(0.9, 0.4, 0.0, 0.0));
	g_material2->SetReflectivityValue(0.1);
	g_material2->SetRoughnessValue(0.9);
	g_material2->SetMetalicityValue(0.0);
	g_material2->SetEmissivenessValue(0.0);
	g_material2->SetPixelShader(g_pixel_shader);
	g_material2->SetVertexShader(g_vertex_shader);
	g_material2->SetFlags((Material::MaterialFlags)0);
	g_obj2->Init(g_mesh2, g_material2);

	theFramework.RegisterObject(g_obj);
	g_obj2->Rotate(float3(1, 0, 0), -3.1415 / 2.0);
	g_obj2->Translate(float3(0, 0, 5));
	theFramework.RegisterObject(g_obj2);

	g_heightmap = new Texture();
	g_heightmap->InitFromFile("Textures/photosculpt-squarebricks-diffuse.dds");
	g_terrainTile = new TerrainTile();
	g_terrainTile->Init(float3(0, 0, 0), float3(1, 1, 1), g_heightmap, 256, 256, g_material);
	theFramework.RegisterObject(g_terrainTile);
}


void CleanUpContent()
{
	/* TODO:
	delete g_obj;
	g_obj = nullptr;

	delete g_mesh;
	g_mesh = nullptr;

	delete g_pixel_shader;
	g_pixel_shader = nullptr;

	delete g_vertex_shader;
	g_vertex_shader = nullptr;

	delete g_diffuse_texture;
	g_diffuse_texture = nullptr;

	delete g_normal_texture;
	g_normal_texture = nullptr;

	delete g_surface_texture;
	g_surface_texture = nullptr;

	delete g_terrainTile;
	g_terrainTile = nullptr;

	delete g_material;
	g_material = nullptr;
	*/
}