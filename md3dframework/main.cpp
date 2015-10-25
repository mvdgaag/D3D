#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "main.h"
#include "RenderContext.h"
#include "Framework.h"
#include "Input.h"
#include "DrawableObject.h"
#include "TerrainTile.h"
#include "Mesh.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Texture.h"
#include "Sampler.h"
#include "Material.h"
#include "Window.h"
#include "Time.h"


Window* g_window = nullptr;

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


HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
void InitContent();
void CleanUpContent();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


static void FrameFunc()
{
	double angle = 0.1 * 2.0 * 3.1415 * theFramework.GetFrameDeltaTime();
	g_obj->Rotate(float3(0, 1, 0), angle);
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_window = new Window();
	
	if (FAILED(g_window->Init(hInstance, nCmdShow)))
	{
		return 0;
	}

	if (FAILED(theRenderContext.Init(g_window)))
	{
		theRenderContext.CleanUp();
		return 0;
	}

	theFramework.Init();
	theFramework.SetFrameCallback(&FrameFunc);

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
			theFramework.Render();
		}
	}

	CleanUpContent();
	theFramework.CleanUp();
	theRenderContext.CleanUp();

	return (int)msg.wParam;
}


void InitContent()
{
	g_obj = new DrawableObject();
	g_obj2 = new DrawableObject();
	
	g_mesh = new Mesh();
	g_mesh->InitFromFile("Models/sphere.obj");

	g_mesh2 = new Mesh();
	g_mesh2->InitPlane(1, 1, float2(8,8));

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
	g_material->SetReflectivityValue(0.5);
	g_material->SetRoughnessValue(0.5);
	g_material->SetMetalicityValue(0.0);
	g_material->SetEmissivenessValue(0.0);
	g_material->SetPixelShader(g_pixel_shader);
	g_material->SetVertexShader(g_vertex_shader);
	g_material->SetFlags(Material::MaterialFlags(0));
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
