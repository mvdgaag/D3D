#include <windows.h>
#include <windowsx.h>
#include "Gaag.h"

#include "Terrain.h"
#include "Mesh.h"
#include "Material.h"
#include "VertexShader.h"
#include "ComputeShader.h"
#include "PixelShader.h"
#include "DrawableObject.h"
#include "Texture.h"

Window* g_window = nullptr;

pDrawableObject g_obj;
pDrawableObject g_obj2;

pMesh g_mesh;
pMesh g_mesh2;

pMaterial g_material;
pMaterial g_material2;

pPixelShader g_pixel_shader;
pVertexShader g_vertex_shader;
pTexture g_diffuse_texture;
pTexture g_normal_texture;
pTexture g_surface_texture;

pTexture g_heightmap;
pTerrainTile g_terrainTile;
pTerrain g_terrain;


HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
void InitContent();
void CleanUpContent();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


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
	g_obj = std::make_shared<DrawableObject>();
	g_obj2 = std::make_shared<DrawableObject>();

	g_mesh = std::make_shared<Mesh>();
	g_mesh->InitFromFile("../md3dFramework/Models/sphere.obj");

	g_mesh2 = std::make_shared<Mesh>();
	g_mesh2->InitPlane(1, 1, float2(8, 8));

	g_pixel_shader = std::make_shared<PixelShader>();
	g_pixel_shader->InitFromFile("../md3dFramework/Shaders/BasicFragmentShader.hlsl");

	g_vertex_shader = std::make_shared<VertexShader>();
	g_vertex_shader->InitFromFile("../md3dFramework/Shaders/BasicVertexShader.hlsl");

	g_diffuse_texture = std::make_shared<Texture>();
	g_diffuse_texture->InitFromFile("../md3dFramework/Textures/photosculpt-squarebricks-diffuse.dds");

	g_normal_texture = std::make_shared<Texture>();
	g_normal_texture->InitFromFile("../md3dFramework/Textures/photosculpt-squarebricks-normal.dds");

	g_surface_texture = std::make_shared<Texture>();
	g_surface_texture->InitFromFile("../md3dFramework/Textures/photosculpt-squarebricks-specular.dds");

	g_material = std::make_shared<Material>();
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

	g_material2 = std::make_shared<Material>();
	g_material2->Init();
	g_material2->SetDiffuseTexture(g_diffuse_texture);
	g_material2->SetNormalTexture(g_normal_texture);
	g_material2->SetSurfaceTexture(g_surface_texture);
	g_material2->SetDiffuseValue(float4(0.9f, 0.4f, 0.0f, 0.0f));
	g_material2->SetReflectivityValue(0.1f);
	g_material2->SetRoughnessValue(0.9f);
	g_material2->SetMetalicityValue(0.0f);
	g_material2->SetEmissivenessValue(0.0f);
	g_material2->SetPixelShader(g_pixel_shader);
	g_material2->SetVertexShader(g_vertex_shader);
	g_material2->SetFlags((Material::MaterialFlags)0);
	g_obj2->Init(g_mesh2, g_material2);

	g_obj2->Rotate(float3(1.0f, 0.0f, 0.0f), -3.1415f / 2.0f);
	g_obj2->Translate(float3(0.0f, 0.0f, 5.0f));

	/*
	g_heightmap = std::make_shared<Texture>();
	g_heightmap->InitFromFile("../md3dFramework/Textures/photosculpt-squarebricks-diffuse.dds");
	g_terrainTile = std::make_shared<TerrainTile>();
	g_terrainTile->Init(float3(0.0f, 0.0f, 0.0f), float3(4.0f, 4.0f, 4.0f), g_heightmap, 256, 256, g_material);
	*/
	
	g_terrain = std::make_shared<Terrain>();
	g_terrain->Init(10, 10, 2, 2, g_material);

	//Gaag.RegisterObject(g_obj);
	//Gaag.RegisterObject(g_obj2);
	//Gaag.RegisterObject(g_terrainTile);
}


void CleanUpContent()
{
	g_obj = nullptr;
	g_mesh = nullptr;
	g_pixel_shader = nullptr;
	g_vertex_shader = nullptr;
	g_diffuse_texture = nullptr;
	g_normal_texture = nullptr;
	g_surface_texture = nullptr;
	g_terrainTile = nullptr;
	g_material = nullptr;
}
