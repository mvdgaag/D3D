#pragma once
#include <windows.h>
#include <windowsx.h>
#include "GaagCommon.h"
#include "resource.h"
#include "Input.h"

PREDEFINE(Window, pWindow);

class Window
{
	friend class RenderContext;

public:
	Window() {};
	~Window() {};
	
	HRESULT Init(HINSTANCE hInstance);

protected:
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	HINSTANCE mHInst = nullptr;
	HWND mHWnd = nullptr;
};
