#include <windows.h>
#include <windowsx.h>
#include "Terrain.h"
#include "SimpleHeightFlow.h"
#include "PaintTool.h"
#include "BrushLibrary.h"
#include "Brush.h"
#include "CameraController.h"
#include "Water.h"
#include "WaterTile.h"
#include "PointLight.h"
#include "SpotLight.h"

Window* g_window = nullptr;

pMeshObject g_obj;
pMesh g_mesh;
pPointLight g_lights[100];
pSpotLight g_spot_light;
pDirectionalLight g_directional_light;

pMaterial g_TerrainMaterial;
pPixelShader g_Terrain_pixel_shader;
pVertexShader g_Terrain_vertex_shader;

pMaterial g_TerrainShadowMaterial;
pPixelShader g_TerrainShadowPixelShader;
pVertexShader g_TerrainShadowVertexShader;

pTexture g_diffuse_texture;
pTexture g_normal_texture;
pTexture g_surface_texture;
pTexture g_heightmap;
pTerrain g_Terrain;
pWater g_water;
pSimpleHeightFlow g_flow;

pPaintTool g_paint_tool;
pBrushLibrary g_brush_library;
pCameraController g_camera_controller;

void FrameFunc();
void InitContent();
void CleanUpContent();


static void FrameFunc()
{
	float time_step = (float)Gaag.GetFrameDeltaTime();

	if (g_water != nullptr)
		g_water->Update(time_step);

	if (g_flow != nullptr)
		g_flow->Update(time_step);
	
	float angle = 0.1f * 2.0f * 3.1415f * time_step;
	g_obj->Rotate(float3(0.0f, 1.0f, 0.0f), angle);

	g_Terrain->ProcessDirtyLayers();

	//angle = 6.283f * glm::fract(theTime.GetTime() / 5.0);
	//g_lights[0]->SetPosition(float3(sin(angle), 1.0, cos(angle)) * 20.0f);

	//g_spot_light->LookAt(float3(sin(angle), 1.0, cos(angle)) * 20.0f);
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Gaag.Init(hInstance, 1200, 900);
	Gaag.SetFrameCallback(&FrameFunc);
	pCamera cam = Gaag.GetCamera();
	cam->SetPosition(50, 100, 50);
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
	g_obj = MAKE_NEW(MeshObject);

	//for (int i = 0; i < 100; i++)
	//{
		//g_lights[i] = theResourceFactory.MakePointLight(float3((i/10 - 5) * 20.0f, 20.0f, (i%10 - 5) * 20.0f), 40.0f, float4(0.1));
		//Gaag.RegisterLight(g_lights[i]);
	//}

	g_directional_light = theResourceFactory.MakeDirectionalLight(float3(1, -1, 1), float4(10, 10, 10, 1), 2048);
	//g_directional_light = theResourceFactory.MakeDirectionalLight(float3(1, -1, 1), float4(10.0, 10.0, 10.0, 1.0), 0);
	Gaag.RegisterLight(g_directional_light);

	//g_spot_light = theResourceFactory.MakeSpotLight(float3(0, 10, 0), 50.0, float3(0, 0, 1), 0.5, float4(1, 0, 0, 1));
	//Gaag.RegisterLight(g_spot_light);

	g_mesh = theResourceFactory.LoadMesh("Models/sphere.obj");
	g_obj->Init(g_mesh, theResourceFactory.GetDefaultMaterial());
	g_obj->Translate(float3(0, 20, 0));
	Gaag.RegisterObject(g_obj);
	
	g_Terrain_pixel_shader = theResourceFactory.LoadPixelShader("Shaders/TerrainFragmentShader.hlsl");
	g_Terrain_vertex_shader = theResourceFactory.LoadVertexShader("Shaders/TerrainVertexShader.hlsl");
	
	g_TerrainMaterial = theResourceFactory.MakeMaterial();
	g_TerrainMaterial->SetDiffuseValue(float4(0.6f, 0.6f, 0.6f, 0.0f));
	g_TerrainMaterial->SetReflectivityValue(0.05f);
	g_TerrainMaterial->SetRoughnessValue(0.8f);
	g_TerrainMaterial->SetMetalicityValue(0.0f);
	g_TerrainMaterial->SetEmissivenessValue(0.0f);
	g_TerrainMaterial->SetPixelShader(g_Terrain_pixel_shader);
	g_TerrainMaterial->SetVertexShader(g_Terrain_vertex_shader);
	
	g_TerrainShadowMaterial = theResourceFactory.MakeMaterial();
	g_TerrainShadowVertexShader = theResourceFactory.LoadVertexShader("Shaders/TerrainShadowVertexShader.hlsl");
	g_TerrainShadowPixelShader = theResourceFactory.LoadPixelShader("Shaders/TerrainShadowFragmentShader.hlsl");
	g_TerrainShadowMaterial->SetVertexShader(g_TerrainShadowVertexShader);
	g_TerrainShadowMaterial->SetPixelShader(g_TerrainShadowPixelShader);

	g_Terrain = MAKE_NEW(Terrain);
	g_Terrain->Init(int2(3), int2(64), float3(50,50,5), g_TerrainMaterial, g_TerrainShadowMaterial);

	/*
	pLayer albedo = new Layer();
	albedo->Init(g_Terrain->GetNumTiles(), g_Terrain->GetTileResolution());
	int albedo_layer_index = g_Terrain->SetLayer(albedo);
	g_Terrain->SetDiffuseLayerIndex(albedo_layer_index);
	*/

	g_flow = MAKE_NEW(SimpleHeightFlow);
	g_flow->Init(g_Terrain);


	/*
	g_water = MAKE_NEW(Water);
	g_water->Init(g_Terrain, g_TerrainMaterial, g_TerrainShadowMaterial);
	int2 num_tiles = g_water->GetTerrain()->GetNumTiles();
	for (int x = 0; x < num_tiles.x; x++)
	{
		for (int y = 0; y < num_tiles.y; y++)
		{
			int2 coord(x, y);
			Material& material = *g_water->GetWaterSurface()->GetTile(coord)->GetMaterial();
			//pTexture texture = g_water->GetTile(coord)->GetFluxTexture();
			//material.SetDiffuseTexture(texture);
			material.SetDiffuseTexture(NULL);
			material.SetNormalTexture(NULL);
			material.SetSurfaceTexture(NULL);
			material.SetDiffuseValue(float4(0.5, 0.5, 0.5, 0.0));
			material.SetReflectivityValue(0.5);
			material.SetRoughnessValue(0.1);
		}
	}
	*/

	g_brush_library = MAKE_NEW(BrushLibrary);
	g_brush_library->Init();

	g_paint_tool = MAKE_NEW(PaintTool);
	g_paint_tool->Init(g_brush_library);
	
	g_paint_tool->SetTargetTerrain(g_Terrain);
	g_paint_tool->SetTargetLayer(g_Terrain->GetHeightLayerIndex());
	//g_paint_tool->SetTargetLayer(g_Terrain->GetDiffuseLayerIndex());

	if (g_flow != nullptr)
	{
		g_paint_tool->SetTargetLayer(g_flow->GetLayerIndex());
		g_Terrain->SetHeightLayerIndex(g_flow->GetLayerIndex());
		//g_Terrain->SetDiffuseLayerIndex(g_Terrain->GetNormalLayerIndex());
		//g_Terrain->SetLayerAsAlbedo(g_flow->GetLayerIndex());
	}

	g_camera_controller = MAKE_NEW(CameraController);
	g_camera_controller->SetTargetCamera(Gaag.GetCamera());
}


void CleanUpContent()
{
	g_obj = nullptr;
	g_mesh = nullptr;
	g_TerrainMaterial = nullptr;
	g_Terrain_pixel_shader = nullptr;
	g_Terrain_vertex_shader = nullptr;
	g_diffuse_texture = nullptr;
	g_normal_texture = nullptr;
	g_surface_texture = nullptr;
	g_heightmap = nullptr;
	g_Terrain = nullptr;
	g_water = nullptr;
	g_paint_tool = nullptr;
	g_brush_library = nullptr;
	g_camera_controller = nullptr;
}
