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

	Gaag.Init(hInstance);
	Gaag.SetFrameCallback(&FrameFunc);
	pCamera cam = Gaag.GetCamera();
	cam->SetPosition(0, 0, -50);
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
	g_mesh = theResourceFactory.LoadMesh("Models/sphere.obj");

	for (int i = 0; i < 100; i++)
	{
		g_materials[i] = theResourceFactory.MakeMaterial();
		g_materials[i]->SetDiffuseValue(float4(1.0, 1.0, 1.0, 0.0f));
		g_materials[i]->SetReflectivityValue(float(i / 10) / 10.0);
		g_materials[i]->SetRoughnessValue(float(i % 10) / 10.0);
		g_materials[i]->SetMetalicityValue(1.0f);
		g_materials[i]->SetEmissivenessValue(0.0f);
		g_materials[i]->SetPixelShader(theResourceFactory.GetDefaultMaterial()->GetPixelShader());
		g_materials[i]->SetVertexShader(theResourceFactory.GetDefaultMaterial()->GetVertexShader());

		g_objects[i] = MAKE_NEW(MeshObject);
		g_objects[i]->Init(g_mesh, theResourceFactory.GetDefaultMaterial());
		g_objects[i]->Init(g_mesh, g_materials[i]);
		g_objects[i]->Translate(float3(3 * (i / 10) - 15, 3 * (i % 10) - 15, 0));
		Gaag.RegisterObject(g_objects[i]);

	}

	g_directional_light = theResourceFactory.MakeDirectionalLight(float3(1, -1, 1), float4(1, 1, 1, 1), 0);
	Gaag.RegisterLight(g_directional_light);

	g_camera_controller = MAKE_NEW(CameraController);
	g_camera_controller->SetTargetCamera(Gaag.GetCamera());
}


void CleanUpContent()
{
	// TODO: safe deletion
}
