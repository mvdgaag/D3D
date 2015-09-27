#pragma once

#include "resource.h"
#include <d3d11_1.h>


#ifndef D3DCall
#ifdef _DEBUG
#define D3DCall(x) \
{ \
	HRESULT hr = x; \
	if (FAILED(hr)) \
	{ \
		OutputDebugStringA(#x); \
		__debugbreak; \
	} \
}
#else
#define D3DCall(x) x
#endif
#endif