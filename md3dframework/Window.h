#pragma once
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "Input.h"


class Window
{
	friend class RenderContext;

public:
	Window() {};
	~Window() {};
	
	HRESULT Init(HINSTANCE hInstance, int nCmdShow);

protected:
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	HINSTANCE mHInst = nullptr;
	HWND mHWnd = nullptr;
};
