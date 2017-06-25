#pragma once
#include <assert.h>
#include <string>
#include <memory>
#include <assert.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include "GaagMath.h"
#include "Rect.h"
#include "Frustum.h"
#include "AABB.h"


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

#ifndef REGISTERTYPE
#define REGISTERTYPE(x) typedef std::vector<x> a##x; typedef std::vector<x*> ap##x;
#endif

#ifndef REGISTERCLASS
#define REGISTERCLASS(x) class x; typedef x* p##x; typedef std::vector<x> a##x; typedef std::vector<x*> ap##x; typedef x& r##x;
#endif

#ifndef REGISTERSTRUCT
#define REGISTERSTRUCT(x) struct x; typedef x* p##x; typedef std::vector<x> a##x; typedef std::vector<x*> ap##x; typedef x& r##x;
#endif

#ifndef MAKE_NEW
#define MAKE_NEW(x) new x();
#endif

REGISTERTYPE(int)
REGISTERTYPE(int2)
REGISTERTYPE(int3)
REGISTERTYPE(int4)
REGISTERTYPE(float)
REGISTERTYPE(float2)
REGISTERTYPE(float3)
REGISTERTYPE(float4)