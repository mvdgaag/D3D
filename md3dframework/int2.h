#pragma once
#include <math.h>
#include "float2.h"


#define USE_GLM
#ifndef USE_GLM



// predeclarations
class int2;


class int2
{
public:
	int x, y;

	inline int2() {}

	inline int2::int2(const int2& vec)
	{
		x = vec.x;
		y = vec.y;
	}

	inline int2::int2(int inX, int inY)
	{
		x = inX;
		y = inY;
	}

	inline int2::int2(int scalar)
	{
		x = scalar;
		y = scalar;
	}

	inline int2::int2(const float2& vec)
	{
		x = vec.x;
		y = vec.y;
	}

	inline int2& int2::operator = (const int2& vec)
	{
		x = vec.x;
		y = vec.y;
		return *this;
	}

	inline bool int2::operator ==(const int2& vec) const
	{
		return ((x == vec.x) && (y == vec.y));
	}

	inline bool int2::operator !=(const int2& vec) const
	{
		return ((x != vec.x) || (y != vec.y));
	}

	inline int & int2::operator [](int idx)
	{
		return *(&x + idx);
	}

	inline int int2::operator [](int idx) const
	{
		return *(&x + idx);
	}

	inline const int2 int2::operator +(const int2& vec) const
	{
		return int2(x + vec.x, y + vec.y);
	}

	inline const int2 int2::operator -(const int2& vec) const
	{
		return int2(x - vec.x, y - vec.y);
	}

	inline const int2 int2::operator +(const int scalar) const
	{
		return int2(x + scalar, y + scalar);
	}

	inline const int2 int2::operator -(const int scalar) const
	{
		return int2(x - scalar, y - scalar);
	}

	inline int2& int2::operator +=(const int2& vec)
	{
		*this = *this + vec;
		return *this;
	}

	inline int2& int2::operator -=(const int2& vec)
	{
		*this = *this - vec;
		return *this;
	}

	inline int2& int2::operator +=(const int scalar)
	{
		*this = *this + scalar;
		return *this;
	}

	inline int2& int2::operator -=(const int scalar)
	{
		*this = *this - scalar;
		return *this;
	}

	inline const int2 int2::operator *(int2 vec) const
	{
		return int2(x * vec.x, y * vec.y);
	}

	inline const int2 int2::operator *(int scalar) const
	{
		return int2(x * scalar, y * scalar);
	}

	inline int2& int2::operator *=(int scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	inline int2& int2::operator *=(int2 vec)
	{
		*this = *this * vec;
		return *this;
	}

	inline const int2 int2::operator /(int2 vec) const
	{
		return int2(x / vec.x, y / vec.y);
	}

	inline const int2 int2::operator /(int scalar) const
	{
		return int2(x / scalar, y / scalar);
	}

	inline int2& int2::operator /=(int2 vec)
	{
		*this = *this / vec;
		return *this;
	}

	inline int2& int2::operator /=(int scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	inline const int2 int2::operator -() const
	{
		return int2(-x, -y);
	}
};


inline void loadXYZ(int2& vec, const int* fptr)
{
	vec = int2(fptr[0], fptr[1]);
}

inline void storeXYZ(const int2& vec, int* fptr)
{
	fptr[0] = vec.x;
	fptr[1] = vec.y;
}

inline const int2 operator *(int scalar, const int2& vec)
{
	return vec * scalar;
}

inline const int2 operator /(int scalar, const int2& vec)
{
	return vec / scalar;
}

inline const int2 operator +(int scalar, const int2& vec)
{
	return vec + scalar;
}

inline const int2 operator -(int scalar, const int2& vec)
{
	return vec - scalar;
}

inline const int2 abs(const int2& vec)
{
	return int2(fabsf(vec.x), fabsf(vec.y));
}

inline const int2 maxPerElem(const int2& vec0, const int2& vec1)
{
	return int2(	(vec0.x > vec1.x) ? vec0.x : vec1.x,
					(vec0.y > vec1.y) ? vec0.y : vec1.y);
}

inline const int2 minPerElem(const int2& vec0, const int2& vec1)
{
	return int2(	(vec0.x < vec1.x) ? vec0.x : vec1.x,
					(vec0.y < vec1.y) ? vec0.y : vec1.y);
}

inline int sumOfElem(const int2& vec)
{
	return vec.x + vec.y;
}

#endif