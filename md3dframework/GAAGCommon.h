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

#ifndef REGISTERCLASS
#define REGISTERCLASS(x) class x; typedef x* p##x; typedef std::vector<x> a##x; typedef std::vector<x*> ap##x; typedef x& r##x;
#endif

#ifndef MAKE_NEW
#define MAKE_NEW(x) new x();
#endif