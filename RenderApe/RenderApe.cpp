#include <windows.h>
#include <windowsx.h>
#include "RenderApe.h"
#include "CameraController.h"

Window* g_window = nullptr;
pMesh g_mesh;
pMaterial g_material;
pMeshObject g_object;
pDirectionalLight g_directional_light;
pCameraController g_camera_controller;

void FrameFunc();
void InitContent();
void CleanUpContent();


static void FrameFunc()
{
	float time_step = (float)Gaag.GetFrameDeltaTime();
	float time = (float)Gaag.GetFrameTime();
	float angle = 0.1f * 2.0f * 3.1415f * time;
	g_object->Rotate(float3(0.0f, 1.0f, 0.0f), angle);
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Gaag.Init(hInstance);
	Gaag.SetFrameCallback(&FrameFunc);
	pCamera cam = Gaag.GetCamera();
	cam->SetPosition(0, 0, -10);
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

	g_object = MAKE_NEW(MeshObject);
	g_object->Init(g_mesh, theResourceFactory.GetDefaultMaterial());
	Gaag.RegisterObject(g_object);
	
	//g_cubemap = theResourceFactory.LoadTexture("Textures/cubemap.dds");

	g_directional_light = theResourceFactory.MakeDirectionalLight(float3(1, -1, 1), float4(1, 1, 1, 1), 0);
	Gaag.RegisterLight(g_directional_light);

	g_camera_controller = MAKE_NEW(CameraController);
	g_camera_controller->SetTargetCamera(Gaag.GetCamera());
}


void CleanUpContent()
{
	// TODO: safe deletion
}
