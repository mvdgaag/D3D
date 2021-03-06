#pragma once

#define USE_GLM
#ifdef USE_GLM

#define GLM_FORCE_RADIANS
#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <vector>

typedef glm::vec2 float2;
typedef float2* pFloat2; 
typedef std::vector<float2> aFloat2;
typedef std::vector<float2*> apFloat2; 
typedef float2& rFloat2;

typedef glm::vec3 float3;
typedef float3* pFloat3;
typedef std::vector<float3> aFloat3;
typedef std::vector<float3*> apFloat3;
typedef float3& rFloat3;

typedef glm::vec4 float4;
typedef float4* pFloat4;
typedef std::vector<float4> aFloat4;
typedef std::vector<float4*> apFloat4;
typedef float4& rFloat4;

typedef glm::ivec2 int2;
typedef int2* pInt2;
typedef std::vector<int2> aInt2;
typedef std::vector<int2*> apInt2;
typedef int2& rInt2;

typedef glm::ivec3 int3;
typedef int3* pInt3;
typedef std::vector<int3> aInt3;
typedef std::vector<int3*> apInt3;
typedef int3& rInt3;

typedef glm::ivec4 int4;
typedef int4* pInt4;
typedef std::vector<int4> aInt4;
typedef std::vector<int4*> apInt4;
typedef int4& rInt4;

typedef glm::mat4 float4x4;
typedef float4x4* pFloat4x4;
typedef std::vector<float4x4> aFloat4x4;
typedef std::vector<float4x4*> apFloat4x4;
typedef float4x4& rFloat4x4;

typedef glm::quat quat;
typedef quat* pQuat;
typedef std::vector<quat> aQuat;
typedef std::vector<quat*> apQuat;
typedef quat& rQuat;

inline float4x4 rotate(const float4x4& inMatrix, float inRadians, const float3& inAxis)
{
	return glm::rotate(inMatrix, inRadians, inAxis);
}

inline float4x4 scale(const float4x4& inMatrix, const float3& inScale)
{
	return glm::scale(inMatrix, inScale);
}

inline float4x4 translate(const float4x4& inMatrix, const float3& inTranslatinn)
{
	return glm::translate(inMatrix, inTranslatinn);
}

inline float4x4 lookAt(const float3& eyePos, const float3& lookAtPos, const float3& upVec)
{
	return glm::lookAt(eyePos, lookAtPos, upVec);
}

inline float4x4 perspective(float fovyRadians, float aspect, float zNear, float zFar)
{
	return glm::perspective(fovyRadians, aspect, zNear, zFar);
}

inline float4x4 ortho(float inLeft, float inRight, float inBottom, float inTop, float inNear, float inFar)
{
	return glm::ortho(inLeft, inRight, inBottom, inTop, inNear, inFar);
}

inline float4x4 transpose(const float4x4& mat)
{
	return glm::transpose(mat);
}

inline float4x4 inverse(const float4x4& mat)
{
	return glm::inverse(mat);
}

inline float length(const float2& a) { return glm::length(a); }
inline float length(const float3& a) { return glm::length(a); }
inline float length(const float4& a) { return glm::length(a); }

inline float3 cross(const float3& a, const float3& b) { return glm::cross(a, b); }

inline float dot(const float2& a, const float2& b) { return glm::dot(a, b); }
inline float dot(const float3& a, const float3& b) { return glm::dot(a, b); }
inline float dot(const float4& a, const float4& b) { return glm::dot(a, b); }

inline float2 normalize(const float2& a) { return glm::normalize(a); }
inline float3 normalize(const float3& a) { return glm::normalize(a); }
inline float4 normalize(const float4& a) { return glm::normalize(a); }

inline float2 minPerElem(const float2& a, const float2& b) { return glm::min(a, b); }
inline float3 minPerElem(const float3& a, const float3& b) { return glm::min(a, b); }
inline float4 minPerElem(const float4& a, const float4& b) { return glm::min(a, b); }
inline int2 minPerElem(const int2& a, const int2& b) { return glm::min(a, b); }
inline int3 minPerElem(const int3& a, const int3& b) { return glm::min(a, b); }
inline int4 minPerElem(const int4& a, const int4& b) { return glm::min(a, b); }
inline float2 maxPerElem(const float2& a, const float2& b) { return glm::max(a, b); }
inline float3 maxPerElem(const float3& a, const float3& b) { return glm::max(a, b); }
inline float4 maxPerElem(const float4& a, const float4& b) { return glm::max(a, b); }
inline int2 maxPerElem(const int2& a, const int2& b) { return glm::max(a, b); }
inline int3 maxPerElem(const int3& a, const int3& b) { return glm::max(a, b); }
inline int4 maxPerElem(const int4& a, const int4& b) { return glm::max(a, b); }

#else // ifdef USE_GLM

#include "float2.h"
#include "float3.h"
#include "float4.h"

#include "int2.h"

#include "float4x4.h"

inline float4x4 rotate(const float4x4& inMatrix, float inRadians, const float3& inAxis)
{
	return inMatrix * makeRotationMatrix(inRadians, inAxis);
}

inline float4x4 scale(const float4x4& inMatrix, const float3& inScale)
{
	return inMatrix * makeScaleMatrix(inScale);
}

inline float4x4 translate(const float4x4& inMatrix, const float3& inTranslatinn)
{
	return inMatrix * makeTranslationMatrix(inTranslatinn);
}

inline float4x4 lookAt(const float3& eyePos, const float3& lookAtPos, const float3& upVec)
{
	return makeLookAtMatrix(eyePos, lookAtPos, upVec);
}

inline float4x4 perspective(float fovyRadians, float aspect, float zNear, float zFar)
{
	return makePerspectiveMatrix(fovyRadians, aspect, zNear, zFar);
}

#endif