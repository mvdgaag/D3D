#include <windows.h>
#include <windowsx.h>
#include "HeightField.h"
#include "PaintTool.h"
#include "BrushLibrary.h"
#include "Brush.h"
#include "CameraController.h"
#include "Water.h"
#include "WaterTile.h"
#include "PointLight.h"

Window* g_window = nullptr;

pDrawableObject g_obj;
pMesh g_mesh;
pPointLight g_lights[100];

pMaterial g_BricksMaterial;
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
	g_water->Update(time_step);
	
	float angle = 0.1f * 2.0f * 3.1415f * time_step;
	g_obj->Rotate(float3(0.0f, 1.0f, 0.0f), angle);

	angle = 6.283f * glm::fract(theTime.GetTime() / 5.0);
	g_lights[0]->SetPosition(float3(sin(angle), 1.0, cos(angle)) * 20.0f);
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

	for (int i = 0; i < 100; i++)
	{
		g_lights[i] = theResourceFactory.MakePointLight(float3((i/10 - 5) * 20.0f, 20.0f, (i%10 - 5) * 20.0f), 20.0f, float4(1.0));
			Gaag.RegisterLight(g_lights[i]);
	}

	g_mesh = theResourceFactory.LoadMesh("Models/sphere.obj");
	g_HeightField_pixel_shader = theResourceFactory.LoadPixelShader("Shaders/TerrainFragmentShader.hlsl");
	g_HeightField_vertex_shader = theResourceFactory.LoadVertexShader("Shaders/TerrainVertexShader.hlsl");
	g_diffuse_texture = theResourceFactory.LoadTexture("Textures/photosculpt-squarebricks-diffuse.dds");
	g_normal_texture = theResourceFactory.LoadTexture("Textures/photosculpt-squarebricks-normal.dds");
	g_surface_texture = theResourceFactory.LoadTexture("Textures/photosculpt-squarebricks-specular.dds");
	g_BricksMaterial = theResourceFactory.MakeMaterial();
	g_BricksMaterial->SetDiffuseTexture(g_diffuse_texture);
	g_BricksMaterial->SetNormalTexture(g_normal_texture);
	g_BricksMaterial->SetSurfaceTexture(g_surface_texture);
	g_BricksMaterial->SetDiffuseValue(float4(0.6f, 0.6f, 0.6f, 0.0f));
	g_BricksMaterial->SetReflectivityValue(0.1f);
	g_BricksMaterial->SetRoughnessValue(0.25f);
	g_BricksMaterial->SetMetalicityValue(0.0f);
	g_BricksMaterial->SetEmissivenessValue(0.0f);
	g_BricksMaterial->SetPixelShader(g_HeightField_pixel_shader);
	g_BricksMaterial->SetVertexShader(g_HeightField_vertex_shader);
	
	g_obj->Init(g_mesh, theResourceFactory.GetDefaultMaterial());
	Gaag.RegisterObject(g_obj);

	g_HeightField = MAKE_NEW(HeightField);
	g_HeightField->Init(int2(3), int2(63), float3(50,50,5), g_BricksMaterial);

	g_water = MAKE_NEW(Water);
	g_water->Init(g_HeightField, g_BricksMaterial);

	int2 num_tiles = g_water->GetTerrainHeightField()->GetNumTiles();
	for (int x = 0; x < num_tiles.x; x++)
	{
		for (int y = 0; y < num_tiles.y; y++)
		{
			int2 coord(x, y);
			pTexture texture = g_water->GetTile(coord)->GetFluxTexture();
			Material& material = *g_water->GetWaterHeightField()->GetTile(coord)->GetMaterial();
			//material.SetDiffuseTexture(texture);
			material.SetDiffuseTexture(NULL);
			material.SetNormalTexture(NULL);
			material.SetSurfaceTexture(NULL);
			material.SetDiffuseValue(float4(1.0, 0.4, 0.0, 0.0));
			material.SetReflectivityValue(0.5);
			material.SetRoughnessValue(0.2);
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
	g_BricksMaterial = nullptr;
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
