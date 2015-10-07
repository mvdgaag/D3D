#pragma once
#include <math.h>


// predeclarations
class float2;
inline float dot(const float2& vec0, const float2& vec1);


class float2
{
public:
	float x, y;

	inline float2() {}

	inline float2::float2(const float2& vec)
	{
		x = vec.x;
		y = vec.y;
	}

	inline float2::float2(float inX, float inY)
	{
		x = inX;
		y = inY;
	}

	inline float2::float2(float scalar)
	{
		x = scalar;
		y = scalar;
	}

	inline float2& float2::operator = (const float2& vec)
	{
		x = vec.x;
		y = vec.y;
		return *this;
	}

	inline bool float2::operator ==(const float2& vec) const
	{
		return ((x == vec.x) && (y == vec.y));
	}

	inline bool float2::operator !=(const float2& vec) const
	{
		return ((x != vec.x) || (y != vec.y));
	}

	inline float & float2::operator [](int idx)
	{
		return *(&x + idx);
	}

	inline float float2::operator [](int idx) const
	{
		return *(&x + idx);
	}

	inline const float2 float2::operator +(const float2& vec) const
	{
		return float2(x + vec.x, y + vec.y);
	}

	inline const float2 float2::operator -(const float2& vec) const
	{
		return float2(x - vec.x, y - vec.y);
	}

	inline const float2 float2::operator +(const float scalar) const
	{
		return float2(x + scalar, y + scalar);
	}

	inline const float2 float2::operator -(const float scalar) const
	{
		return float2(x - scalar, y - scalar);
	}

	inline float2& float2::operator +=(const float2& vec)
	{
		*this = *this + vec;
		return *this;
	}

	inline float2& float2::operator -=(const float2& vec)
	{
		*this = *this - vec;
		return *this;
	}

	inline float2& float2::operator +=(const float scalar)
	{
		*this = *this + scalar;
		return *this;
	}

	inline float2& float2::operator -=(const float2 scalar)
	{
		*this = *this - scalar;
		return *this;
	}

	inline const float2 float2::operator *(float2 vec) const
	{
		return float2(x * vec.x, y * vec.y);
	}

	inline const float2 float2::operator *(float scalar) const
	{
		return float2(x * scalar, y * scalar);
	}

	inline float2& float2::operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	inline float2& float2::operator *=(float2 vec)
	{
		*this = *this * vec;
		return *this;
	}

	inline const float2 float2::operator /(float2 vec) const
	{
		return float2(x / vec.x, y / vec.y);
	}

	inline const float2 float2::operator /(float scalar) const
	{
		return float2(x / scalar, y / scalar);
	}

	inline float2& float2::operator /=(float2 vec)
	{
		*this = *this / vec;
		return *this;
	}

	inline float2& float2::operator /=(float scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	inline const float2 float2::operator -() const
	{
		return float2(-x, -y);
	}
};


inline const float2 lerp(float t, const float2& vec0, const float2& vec1)
{
	return (vec0 + ((vec1 - vec0) * t));
}

inline const float2 slerp(float t, const float2& unitVec0, const float2& unitVec1)
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

inline void loadXYZ(float2& vec, const float* fptr)
{
	vec = float2(fptr[0], fptr[1]);
}

inline void storeXYZ(const float2& vec, float* fptr)
{
	fptr[0] = vec.x;
	fptr[1] = vec.y;
}

inline const float2 operator *(float scalar, const float2& vec)
{
	return vec * scalar;
}

inline const float2 operator /(float scalar, const float2& vec)
{
	return vec / scalar;
}

inline const float2 operator +(float scalar, const float2& vec)
{
	return vec + scalar;
}

inline const float2 operator -(float scalar, const float2& vec)
{
	return vec - scalar;
}

inline const float2 sqrt(const float2& vec)
{
	return float2(sqrtf(vec.x), sqrtf(vec.y));
}

inline const float2 abs(const float2& vec)
{
	return float2(fabsf(vec.x), fabsf(vec.y));
}

inline const float2 maxPerElem(const float2& vec0, const float2& vec1)
{
	return float2(	(vec0.x > vec1.x) ? vec0.x : vec1.x,
					(vec0.y > vec1.y) ? vec0.y : vec1.y);
}

inline const float2 minPerElem(const float2& vec0, const float2& vec1)
{
	return float2(	(vec0.x < vec1.x) ? vec0.x : vec1.x,
					(vec0.y < vec1.y) ? vec0.y : vec1.y);
}

inline float sumOfElem(const float2& vec)
{
	return vec.x + vec.y;
}

inline float dot(const float2& vec0, const float2& vec1)
{
	return (vec0.x * vec1.x) + (vec0.y * vec1.y);
}

inline float length(const float2& vec)
{
	return ::sqrtf(dot(vec, vec));
}

inline const float2 normalize(const float2& vec)
{
	float rcpLength = 1.0f / length(vec);
	return vec * rcpLength;
}
