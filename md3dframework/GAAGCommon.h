#pragma once
#include <string>
#include <memory>
#include <assert.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "GaagMath.h"
#include "rect.h"


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

#ifndef PREDEFINE
#define PREDEFINE(x, px) class x; typedef std::shared_ptr<x> px;
#endif

#ifndef MAKE_NEW
#define MAKE_NEW(x) std::make_shared<x>();
#endif