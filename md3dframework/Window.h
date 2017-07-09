#pragma once
#include <windows.h>
#include <windowsx.h>
#include "GaagCommon.h"
#include "Input.h"

REGISTERCLASS(Window);

class Window
{
	friend class RenderContext;

public:
	Window() {};
	~Window() {};
	
	HRESULT Init(HINSTANCE hInstance, int inWidth, int inHeight);
	HRESULT Init(HWND inWindowHandle);
	HWND GetHandle()			{ return mHWnd; }
	HINSTANCE GetInstance()		{ return mHInst; }
	
protected:
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	HINSTANCE mHInst = nullptr;
	HWND mHWnd = nullptr;
	int mWidth;
	int mHeight;
};
