#include <windows.h>
#include <windowsx.h>
#include "Terrain.h"
#include "PaintTool.h"
#include "Brush.h"

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

pPaintTool g_paint_tool;
pBrush g_brush;

void FrameFunc();
void InitContent();
void CleanUpContent();


static void FrameFunc()
{
	float angle = 0.1f * 2.0f * 3.1415f * (float)Gaag.GetFrameDeltaTime();
	g_obj->Rotate(float3(0.0f, 1.0f, 0.0f), angle);
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Gaag.Init(hInstance);
	Gaag.SetFrameCallback(&FrameFunc);
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
	g_material->SetDiffuseValue(float4(0.6f, 0.4f, 0.0f, 0.0f));
	g_material->SetReflectivityValue(0.5f);
	g_material->SetRoughnessValue(0.5f);
	g_material->SetMetalicityValue(0.0f);
	g_material->SetEmissivenessValue(0.0f);
	g_material->SetPixelShader(g_pixel_shader);
	g_material->SetVertexShader(g_vertex_shader);
	//g_material->SetFlags(Material::MaterialFlags(0));
	g_obj->Init(g_mesh, g_material);
	Gaag.RegisterObject(g_obj);

	g_terrain = MAKE_NEW(Terrain);
	g_terrain->Init(int2(3), int2(256), float3(5,5,1), g_material);

	g_brush = MAKE_NEW(Brush);

	g_paint_tool = MAKE_NEW(PaintTool);
	g_paint_tool->SetBrush(g_brush);
	g_paint_tool->SetTargetTerrain(g_terrain);
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
