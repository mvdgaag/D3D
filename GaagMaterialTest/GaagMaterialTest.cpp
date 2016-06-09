#include <windows.h>
#include <windowsx.h>
#include "GaagMaterialTest.h"
#include "CameraController.h"
#include "PointLight.h"
#include "SpotLight.h"

Window* g_window = nullptr;

pMeshObject g_objects[100];
pMesh g_mesh;
pDirectionalLight g_directional_light;
pMaterial g_materials[100];
pCameraController g_camera_controller;
pTexture g_cubemap;
pTexture g_filtered_cubemap;

void FrameFunc();
void InitContent();
void CleanUpContent();


static void FrameFunc()
{
	float time_step = (float)Gaag.GetFrameDeltaTime();
	float time = (float)Gaag.GetFrameTime();
	//float angle = 0.1f * 2.0f * 3.1415f * time;
	//g_obj->Rotate(float3(0.0f, 1.0f, 0.0f), angle);
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Gaag.Init(hInstance, 800, 600);
	Gaag.SetFrameCallback(&FrameFunc);
	
	pCamera cam = Gaag.GetCamera();
	cam->SetPosition(5, 5, -15);
	InitContent();

	RenderState render_state;
	render_state.EnableDirect = true;
	render_state.EnableIndirect = true;
	render_state.EnableReflections = true;
	render_state.EnablePostProcess = true;
	render_state.CubeMap = g_filtered_cubemap;
	//render_state.CubeMap = g_cubemap;
	Gaag.SetRenderState(render_state);

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
	g_mesh = theResourceFactory.LoadMesh("Models/sphere.obj");

	for (int i = 0; i < 25; i++)
	{
		g_materials[i] = theResourceFactory.MakeMaterial();
		g_materials[i]->SetDiffuseValue(float4(0.9, 0.0, 0.0, 0.0f));
		g_materials[i]->SetReflectivityValue(fmaxf(0.04f, float(i / 5) / 5.0)); // low reflectance row = 4% (typical for non-metals)
		g_materials[i]->SetRoughnessValue(pow(float(i % 5) / 5.0, 1.0));
		g_materials[i]->SetMetalicityValue(0.0);
		g_materials[i]->SetEmissivenessValue(0.0f);
		g_materials[i]->SetPixelShader(theResourceFactory.GetDefaultMaterial()->GetPixelShader());
		g_materials[i]->SetVertexShader(theResourceFactory.GetDefaultMaterial()->GetVertexShader());

		g_objects[i] = MAKE_NEW(MeshObject);
		g_objects[i]->Init(g_mesh, theResourceFactory.GetDefaultMaterial());
		g_objects[i]->Init(g_mesh, g_materials[i]);
		g_objects[i]->Translate(float3(3 * (i / 5) - 7.5, 3 * (i % 5) - 7.5, 0));
		Gaag.RegisterObject(g_objects[i]);
	}

	g_cubemap = theResourceFactory.LoadTexture("Textures/cubemap.dds", BIND_COMPUTE_TARGET);
	
	// TODO: even if the cubemap is not used, the context seems to crash on this?
	g_filtered_cubemap = Gaag.PreFilterCubemap(g_cubemap);

	g_directional_light = theResourceFactory.MakeDirectionalLight(float3(1, -1, 1), float4(1, 1, 1, 1), 0);
	Gaag.RegisterLight(g_directional_light);

	g_camera_controller = MAKE_NEW(CameraController);
	g_camera_controller->SetTargetCamera(Gaag.GetCamera());
}


void CleanUpContent()
{
	// TODO: safe deletion
}
