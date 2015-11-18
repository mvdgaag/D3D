#include <windows.h>
#include <windowsx.h>
#include "Terrain.h"
#include "PaintTool.h"
#include "BrushLibrary.h"
#include "Brush.h"
#include "CameraController.h"
#include "WaterTile.h"

Window* g_window = nullptr;

pDrawableObject g_obj;
pMesh g_mesh;
pMaterial g_material;
pPixelShader g_pixel_shader;
pVertexShader g_vertex_shader;
pTexture g_diffuse_texture;
pTexture g_normal_texture;
pTexture g_surface_texture;
pTexture g_heightmap;
pTerrain g_terrain;
pWaterTile g_water_tile;

pPaintTool g_paint_tool;
pBrushLibrary g_brush_library;
pCameraController g_camera_controller;

void FrameFunc();
void InitContent();
void CleanUpContent();


static void FrameFunc()
{
	float angle = 0.1f * 2.0f * 3.1415f * (float)Gaag.GetFrameDeltaTime();
	g_obj->Rotate(float3(0.0f, 1.0f, 0.0f), angle);

	g_water_tile->Update();
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Gaag.Init(hInstance);
	Gaag.SetFrameCallback(&FrameFunc);
	pCamera cam = Gaag.GetCamera();
	cam->SetPosition(15, 30, 15);
	InitContent();

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
			Gaag.Render();
		}
	}

	CleanUpContent();
	Gaag.CleanUp();

	return (int)msg.wParam;
}


void InitContent()
{
	g_obj = MAKE_NEW(DrawableObject);

	g_mesh = MAKE_NEW(Mesh);
	g_mesh->InitFromFile("Models/sphere.obj");

	g_pixel_shader = MAKE_NEW(PixelShader);
	g_pixel_shader->InitFromFile("Shaders/TerrainFragmentShader.hlsl");

	g_vertex_shader = MAKE_NEW(VertexShader);
	g_vertex_shader->InitFromFile("Shaders/TerrainVertexShader.hlsl");

	g_diffuse_texture = MAKE_NEW(Texture);
	g_diffuse_texture->InitFromFile("Textures/photosculpt-squarebricks-diffuse.dds");

	g_normal_texture = MAKE_NEW(Texture);
	g_normal_texture->InitFromFile("Textures/photosculpt-squarebricks-normal.dds");

	g_surface_texture = MAKE_NEW(Texture);
	g_surface_texture->InitFromFile("Textures/photosculpt-squarebricks-specular.dds");

	g_material = MAKE_NEW(Material);
	g_material->Init();
	g_material->SetDiffuseTexture(g_diffuse_texture);
	g_material->SetNormalTexture(g_normal_texture);
	g_material->SetSurfaceTexture(g_surface_texture);
	g_material->SetDiffuseValue(float4(0.6f, 0.6f, 0.6f, 0.0f));
	g_material->SetReflectivityValue(0.1f);
	g_material->SetRoughnessValue(0.9f);
	g_material->SetMetalicityValue(0.0f);
	g_material->SetEmissivenessValue(0.0f);
	g_material->SetPixelShader(g_pixel_shader);
	g_material->SetVertexShader(g_vertex_shader);
	//g_material->SetFlags(Material::MaterialFlags(0));
	g_obj->Init(g_mesh, Gaag.GetDefaultMaterial());
	Gaag.RegisterObject(g_obj);

	g_terrain = MAKE_NEW(Terrain);
	g_terrain->Init(int2(3), int2(64), float3(10,10,1), g_material);
	g_terrain->GetTile(int2(1, 1));
	
	g_brush_library = MAKE_NEW(BrushLibrary);
	g_brush_library->Init();

	g_paint_tool = MAKE_NEW(PaintTool);
	g_paint_tool->Init(g_brush_library);
	g_paint_tool->SetTargetTerrain(g_terrain);

	g_camera_controller = MAKE_NEW(CameraController);
	g_camera_controller->SetTargetCamera(Gaag.GetCamera());

	g_water_tile = MAKE_NEW(WaterTile);
	pTerrainTile t1 = g_terrain->GetTile(float2(0, 0));
	pTerrainTile t2 = g_terrain->GetTile(float2(10, 10));
	g_water_tile->Init(t1->GetTexture(), t2->GetTexture());
}


void CleanUpContent()
{
	g_terrain = nullptr;
	g_obj = nullptr;
	g_mesh = nullptr;
	g_pixel_shader = nullptr;
	g_vertex_shader = nullptr;
	g_diffuse_texture = nullptr;
	g_normal_texture = nullptr;
	g_surface_texture = nullptr;
	g_heightmap = nullptr;
	g_material = nullptr;
}
