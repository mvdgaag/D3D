#include <windows.h>
#include <windowsx.h>
#include "HeightField.h"
#include "PaintTool.h"
#include "BrushLibrary.h"
#include "Brush.h"
#include "CameraController.h"
#include "Water.h"
#include "WaterTile.h"

Window* g_window = nullptr;

pDrawableObject g_obj;
pMesh g_mesh;

pMaterial g_HeightField_material;
pPixelShader g_HeightField_pixel_shader;
pVertexShader g_HeightField_vertex_shader;

pTexture g_diffuse_texture;
pTexture g_normal_texture;
pTexture g_surface_texture;
pTexture g_heightmap;
pHeightField g_HeightField;
pWater g_water;

pPaintTool g_paint_tool;
pBrushLibrary g_brush_library;
pCameraController g_camera_controller;

void FrameFunc();
void InitContent();
void CleanUpContent();


static void FrameFunc()
{
	float time_step = (float)Gaag.GetFrameDeltaTime();
	float angle = 0.1f * 2.0f * 3.1415f * time_step;
	g_obj->Rotate(float3(0.0f, 1.0f, 0.0f), angle);

	g_water->Update(time_step);
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

	g_HeightField_pixel_shader = MAKE_NEW(PixelShader);
	g_HeightField_pixel_shader->InitFromFile("Shaders/TerrainFragmentShader.hlsl");

	g_HeightField_vertex_shader = MAKE_NEW(VertexShader);
	g_HeightField_vertex_shader->InitFromFile("Shaders/TerrainVertexShader.hlsl");

	g_diffuse_texture = MAKE_NEW(Texture);
	g_diffuse_texture->InitFromFile("Textures/photosculpt-squarebricks-diffuse.dds");

	g_normal_texture = MAKE_NEW(Texture);
	g_normal_texture->InitFromFile("Textures/photosculpt-squarebricks-normal.dds");

	g_surface_texture = MAKE_NEW(Texture);
	g_surface_texture->InitFromFile("Textures/photosculpt-squarebricks-specular.dds");

	g_HeightField_material = MAKE_NEW(Material);
	g_HeightField_material->Init();
	g_HeightField_material->SetDiffuseTexture(g_diffuse_texture);
	g_HeightField_material->SetNormalTexture(g_normal_texture);
	g_HeightField_material->SetSurfaceTexture(g_surface_texture);
	g_HeightField_material->SetDiffuseValue(float4(0.6f, 0.6f, 0.6f, 0.0f));
	g_HeightField_material->SetReflectivityValue(0.5f);
	g_HeightField_material->SetRoughnessValue(0.25f);
	g_HeightField_material->SetMetalicityValue(0.0f);
	g_HeightField_material->SetEmissivenessValue(0.0f);
	g_HeightField_material->SetPixelShader(g_HeightField_pixel_shader);
	g_HeightField_material->SetVertexShader(g_HeightField_vertex_shader);
	
	g_obj->Init(g_mesh, Gaag.GetDefaultMaterial());
	Gaag.RegisterObject(g_obj);

	g_HeightField = MAKE_NEW(HeightField);
	g_HeightField->Init(int2(3), int2(63), float3(50,50,5), g_HeightField_material);

	g_water = MAKE_NEW(Water);
	g_water->Init(g_HeightField, g_HeightField_material);

	int2 num_tiles = g_water->GetTerrainHeightField()->GetNumTiles();
	for (int x = 0; x < num_tiles.x; x++)
	{
		for (int y = 0; y < num_tiles.y; y++)
		{
			int2 coord(x, y);
			pTexture texture = g_water->GetTile(coord)->GetFluxTexture();
			Material& material = *g_water->GetWaterHeightField()->GetTile(coord)->GetMaterial();
			material.SetDiffuseTexture(texture);
			material.SetNormalTexture(NULL);
			material.SetSurfaceTexture(NULL);
		}
	}

	g_brush_library = MAKE_NEW(BrushLibrary);
	g_brush_library->Init();

	g_paint_tool = MAKE_NEW(PaintTool);
	g_paint_tool->Init(g_brush_library);
	g_paint_tool->SetTargetHeightField(g_HeightField);

	g_camera_controller = MAKE_NEW(CameraController);
	g_camera_controller->SetTargetCamera(Gaag.GetCamera());
}


void CleanUpContent()
{
	g_obj = nullptr;
	g_mesh = nullptr;
	g_HeightField_material = nullptr;
	g_HeightField_pixel_shader = nullptr;
	g_HeightField_vertex_shader = nullptr;
	g_diffuse_texture = nullptr;
	g_normal_texture = nullptr;
	g_surface_texture = nullptr;
	g_heightmap = nullptr;
	g_HeightField = nullptr;
	g_water = nullptr;
	g_paint_tool = nullptr;
	g_brush_library = nullptr;
	g_camera_controller = nullptr;
}
