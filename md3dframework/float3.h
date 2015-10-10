#pragma once
#include <math.h>

// predeclarations
class float3;
inline float dot(const float3& vec0, const float3& vec1);


class float3
{
public:
	float x, y, z;

	inline float3() {}

	inline float3::float3(const float3& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}

	inline float3::float3(float inX, float inY, float inZ)
	{
		x = inX;
		y = inY;
		z = inZ;
	}

	inline float3::float3(float scalar)
	{
		x = scalar;
		y = scalar;
		z = scalar;
	}

	inline float3& float3::operator = (const float3& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		return *this;
	}

	inline float2 float3::XY() const { return float2(x, y); }

	inline bool float3::operator ==(const float3& vec) const
	{
		return ((x == vec.x) && (y == vec.y) && (z == vec.z));
	}

	inline bool float3::operator !=(const float3& vec) const
	{
		return ((x != vec.x) || (y != vec.y) || (z != vec.z));
	}

	inline float & float3::operator [](int idx)
	{
		return *(&x + idx);
	}

	inline float float3::operator [](int idx) const
	{
		return *(&x + idx);
	}

	inline const float3 float3::operator +(const float3& vec) const
	{
		return float3(x + vec.x, y + vec.y, z + vec.z);
	}

	inline const float3 float3::operator -(const float3& vec) const
	{
		return float3(x - vec.x, y - vec.y, z - vec.z);
	}

	inline const float3 float3::operator +(const float scalar) const
	{
		return float3(x + scalar, y + scalar, z + scalar);
	}

	inline const float3 float3::operator -(const float scalar) const
	{
		return float3(x - scalar, y - scalar, z - scalar);
	}

	inline float3& float3::operator +=(const float3& vec)
	{
		*this = *this + vec;
		return *this;
	}

	inline float3& float3::operator -=(const float3& vec)
	{
		*this = *this - vec;
		return *this;
	}

	inline float3& float3::operator +=(const float scalar)
	{
		*this = *this + scalar;
		return *this;
	}

	inline float3& float3::operator -=(const float3 scalar)
	{
		*this = *this - scalar;
		return *this;
	}

	inline const float3 float3::operator *(float3 vec) const
	{
		return float3(x * vec.x, y * vec.y, z * vec.z);
	}

	inline const float3 float3::operator *(float scalar) const
	{
		return float3(x * scalar, y * scalar, z * scalar);
	}

	inline float3& float3::operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	inline float3& float3::operator *=(float3 vec)
	{
		*this = *this * vec;
		return *this;
	}

	inline const float3 float3::operator /(float3 vec) const
	{
		return float3(x / vec.x, y / vec.y, z / vec.z);
	}

	inline const float3 float3::operator /(float scalar) const
	{
		return float3(x / scalar, y / scalar, z / scalar);
	}

	inline float3& float3::operator /=(float3 vec)
	{
		*this = *this / vec;
		return *this;
	}

	inline float3& float3::operator /=(float scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	inline const float3 float3::operator -() const
	{
		return float3(-x, -y, -z);
	}
};


inline const float3 lerp(float t, const float3& vec0, const float3& vec1)
{
	return (vec0 + ((vec1 - vec0) * t));
}

inline const float3 slerp(float t, const float3& unitVec0, const float3& unitVec1)
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

inline void loadXYZ(float3& vec, const float* fptr)
{
	vec = float3(fptr[0], fptr[1], fptr[2]);
}

inline void storeXYZ(const float3& vec, float* fptr)
{
	fptr[0] = vec.x;
	fptr[1] = vec.y;
	fptr[2] = vec.z;
}

inline const float3 operator *(float scalar, const float3& vec)
{
	return vec * scalar;
}

inline const float3 operator /(float scalar, const float3& vec)
{
	return vec / scalar;
}

inline const float3 operator +(float scalar, const float3& vec)
{
	return vec + scalar;
}

inline const float3 operator -(float scalar, const float3& vec)
{
	return vec - scalar;
}

inline const float3 sqrt(const float3& vec)
{
	return float3(sqrtf(vec.x), sqrtf(vec.y), sqrtf(vec.z));
}

inline const float3 abs(const float3& vec)
{
	return float3(fabsf(vec.x), fabsf(vec.y), fabsf(vec.z));
}

inline const float3 maxPerElem(const float3& vec0, const float3& vec1)
{
	return float3(	(vec0.x > vec1.x) ? vec0.x : vec1.x,
					(vec0.y > vec1.y) ? vec0.y : vec1.y,
					(vec0.z > vec1.z) ? vec0.z : vec1.z);
}

inline const float3 minPerElem(const float3& vec0, const float3& vec1)
{
	return float3(	(vec0.x < vec1.x) ? vec0.x : vec1.x,
					(vec0.y < vec1.y) ? vec0.y : vec1.y,
					(vec0.z < vec1.z) ? vec0.z : vec1.z);
}

inline float sumOfElem(const float3& vec)
{
	return vec.x + vec.y + vec.z;
}

inline float dot(const float3& vec0, const float3& vec1)
{
	return (vec0.x * vec1.x) + (vec0.y * vec1.y) + (vec0.z * vec1.z);
}

inline float length(const float3& vec)
{
	return ::sqrtf(dot(vec, vec));
}

inline const float3 normalize(const float3& vec)
{
	float rcpLength = 1.0f / length(vec);
	return vec * rcpLength;
}

inline const float3 cross(const float3& vec0, const float3& vec1)
{
	return float3(	(vec0.y * vec1.z) - (vec0.z * vec1.y),
					(vec0.z * vec1.x) - (vec0.x * vec1.z),
					(vec0.x * vec1.y) - (vec0.y * vec1.x));
}
