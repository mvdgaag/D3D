#include "Window.h"
#include "RenderContext.h"

HRESULT Window::Init(HINSTANCE hInstance)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Window::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"Gaag";
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	mHInst = hInstance;
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	mHWnd = CreateWindow(L"Gaag", L"Gaag",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!mHWnd)
		return E_FAIL;

	ShowWindow(mHWnd, SW_SHOW);

	return S_OK;
}


LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		theInput.OnKeyDown(wParam);
		break;

	case WM_KEYUP:
		theInput.OnKeyUp(wParam);
		break;

	case WM_MOUSEMOVE:
		theInput.OnMouseMove(float2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;

	case WM_LBUTTONDOWN:
		theInput.OnMouseDown(0);
		break;

	case WM_LBUTTONUP:
		theInput.OnMouseUp(0);
		break;

	case WM_RBUTTONDOWN:
		theInput.OnMouseDown(1);
		break;

	case WM_RBUTTONUP:
		theInput.OnMouseUp(1);
		break;

	case WM_MBUTTONDOWN:
		theInput.OnMouseDown(2);
		break;

	case WM_MBUTTONUP:
		theInput.OnMouseUp(2);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
