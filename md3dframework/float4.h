#pragma once
#include <math.h>
#include "float3.h"
#include "float2.h"


// predeclarations
class float4;
inline float dot(const float4& vec0, const float4& vec1);


class float4
{
public:
	float x, y, z, w;

	inline float4() {}

	inline float4::float4(const float4& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = vec.w;
	}

	inline float4::float4(const float3& vec, float inW)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = inW;
	}

	inline float4::float4(float inX, float inY, float inZ, float inW)
	{
		x = inX;
		y = inY;
		z = inZ;
		w = inW;
	}

	inline float4::float4(float scalar)
	{
		x = scalar;
		y = scalar;
		z = scalar;
		w = scalar;
	}

	inline float4& float4::operator = (const float4& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = vec.w;
		return *this;
	}

	inline float & float4::operator [](int idx)
	{
		return *(&x + idx);
	}

	inline float float4::operator [](int idx) const
	{
		return *(&x + idx);
	}

	inline const float4 float4::operator +(const float4& vec) const
	{
		return float4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
	}

	inline const float4 float4::operator -(const float4& vec) const
	{
		return float4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
	}

	inline const float4 float4::operator +(const float scalar) const
	{
		return float4(x + scalar, y + scalar, z + scalar, w + scalar);
	}

	inline const float4 float4::operator -(const float scalar) const
	{
		return float4(x - scalar, y - scalar, z - scalar, w - scalar);
	}

	inline float4& float4::operator +=(const float4& vec)
	{
		*this = *this + vec;
		return *this;
	}

	inline float4& float4::operator -=(const float4& vec)
	{
		*this = *this - vec;
		return *this;
	}

	inline float4& float4::operator +=(const float scalar)
	{
		*this = *this + scalar;
		return *this;
	}

	inline float4& float4::operator -=(const float4 scalar)
	{
		*this = *this - scalar;
		return *this;
	}

	inline const float4 float4::operator *(float4 vec) const
	{
		return float4(x * vec.x, y * vec.y, z * vec.z, w * vec.w);
	}

	inline const float4 float4::operator *(float scalar) const
	{
		return float4(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	inline float4& float4::operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	inline float4& float4::operator *=(float4 vec)
	{
		*this = *this * vec;
		return *this;
	}

	inline const float4 float4::operator /(float4 vec) const
	{
		return float4(x / vec.x, y / vec.y, z / vec.z, w / vec.w);
	}

	inline const float4 float4::operator /(float scalar) const
	{
		return float4(x / scalar, y / scalar, z / scalar, w / scalar);
	}

	inline float4& float4::operator /=(float4 vec)
	{
		*this = *this / vec;
		return *this;
	}

	inline float4& float4::operator /=(float scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	inline const float4 float4::operator -() const
	{
		return float4(-x, -y, -z, -w);
	}

	inline const float2 XY() const
	{
		return float2(x, y);
	}

	inline const float3 XYZ() const
	{
		return float3(x, y, z);
	}
};

inline const float4 lerp(float t, const float4& vec0, const float4& vec1)
{
	return (vec0 + ((vec1 - vec0) * t));
}

inline const float4 slerp(float t, const float4& unitVec0, const float4& unitVec1)
{
	float recipSinAngle, scale0, scale1, cosAngle, angle;
	cosAngle = dot(unitVec0, unitVec1);
	if (cosAngle < 0.999)
	{
		angle = acosf(cosAngle);
		recipSinAngle = (1.0f / sinf(angle));
		scale0 = (sinf(((1.0f - t) * angle)) * recipSinAngle);
		scale1 = (sinf((t * angle)) * recipSinAngle);
	}
	else
	{
		scale0 = (1.0f - t);
		scale1 = t;
	}
	return ((unitVec0 * scale0) + (unitVec1 * scale1));
}

inline void loadXYZ(float4& vec, const float* fptr)
{
	vec = float4(fptr[0], fptr[1], fptr[2], fptr[3]);
}

inline void storeXYZ(const float4& vec, float* fptr)
{
	fptr[0] = vec.x;
	fptr[1] = vec.y;
	fptr[2] = vec.z;
	fptr[3] = vec.w;
}

inline const float4 operator *(float scalar, const float4& vec)
{
	return vec * scalar;
}

inline const float4 operator /(float scalar, const float4& vec)
{
	return vec / scalar;
}

inline const float4 operator +(float scalar, const float4& vec)
{
	return vec + scalar;
}

inline const float4 operator -(float scalar, const float4& vec)
{
	return vec - scalar;
}

inline const float4 sqrt(const float4& vec)
{
	return float4(sqrtf(vec.x), sqrtf(vec.y), sqrtf(vec.z), sqrtf(vec.w));
}

inline const float4 abs(const float4& vec)
{
	return float4(fabsf(vec.x), fabsf(vec.y), fabsf(vec.z), fabsf(vec.w));
}

inline const float4 maxPerElem(const float4& vec0, const float4& vec1)
{
	return float4(	(vec0.x > vec1.x) ? vec0.x : vec1.x,
					(vec0.y > vec1.y) ? vec0.y : vec1.y,
					(vec0.z > vec1.z) ? vec0.z : vec1.z,
					(vec0.w > vec1.w) ? vec0.w : vec1.w);
}

inline const float4 minPerElem(const float4& vec0, const float4& vec1)
{
	return float4(	(vec0.x < vec1.x) ? vec0.x : vec1.x,
					(vec0.y < vec1.y) ? vec0.y : vec1.y,
					(vec0.z < vec1.z) ? vec0.z : vec1.z,
					(vec0.w < vec1.w) ? vec0.w : vec1.w);
}

inline float sumOfElem(const float4& vec)
{
	return vec.x + vec.y + vec.z + vec.w;
}

inline float dot(const float4& vec0, const float4& vec1)
{
	return (vec0.x * vec1.x) + (vec0.y * vec1.y) + (vec0.z * vec1.z) + (vec0.w * vec1.w);
}

inline float length(const float4& vec)
{
	return ::sqrtf(dot(vec, vec));
}

inline const float4 normalize(const float4& vec)
{
	float rcpLength = 1.0f / length(vec);
	return vec * rcpLength;
}
