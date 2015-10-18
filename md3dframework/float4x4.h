#pragma once
#include "float3.h"
#include "float4.h"

// predeclarations
class float4x4;
inline const float4x4 orthoInverse(const float4x4 & tfrm);


class float4x4
{
public:
	float4 mCol0;
	float4 mCol1;
	float4 mCol2;
	float4 mCol3;

	inline float4x4() { };

	inline float4x4::float4x4(const float4x4& mat)
	{
		mCol0 = mat.mCol0;
		mCol1 = mat.mCol1;
		mCol2 = mat.mCol2;
		mCol3 = mat.mCol3;
	}

	inline float4x4::float4x4(float scalar)
	{
		mCol0 = float4(scalar);
		mCol1 = float4(scalar);
		mCol2 = float4(scalar);
		mCol3 = float4(scalar);
	}

	inline float4x4::float4x4(const float4& inCol0, const float4& inCol1, const float4& inCol2, const float4& inCol3)
	{
		mCol0 = inCol0;
		mCol1 = inCol1;
		mCol2 = inCol2;
		mCol3 = inCol3;
	}

	inline float4x4& float4x4::setRow(int row, const float4& vec)
	{
		mCol0[row] = vec.x;
		mCol1[row] = vec.y;
		mCol2[row] = vec.z;
		mCol3[row] = vec.w;
		return *this;
	}

	inline float4& float4x4::operator [](int col)
	{
		return *(&mCol0 + col);
	}

	inline const float4 float4x4::operator [](int col) const
	{
		return *(&mCol0 + col);
	}

	inline float4x4& float4x4::operator =(const float4x4& mat)
	{
		mCol0 = mat.mCol0;
		mCol1 = mat.mCol1;
		mCol2 = mat.mCol2;
		mCol3 = mat.mCol3;
		return *this;
	}

	inline const float4x4 float4x4::operator +(const float4x4& mat) const
	{
		return float4x4(
			(mCol0 + mat.mCol0),
			(mCol1 + mat.mCol1),
			(mCol2 + mat.mCol2),
			(mCol3 + mat.mCol3)
			);
	}

	inline const float4x4 float4x4::operator -(const float4x4& mat) const
	{
		return float4x4(
			(mCol0 - mat.mCol0),
			(mCol1 - mat.mCol1),
			(mCol2 - mat.mCol2),
			(mCol3 - mat.mCol3)
			);
	}

	inline float4x4& float4x4::operator +=(const float4x4& mat)
	{
		*this = *this + mat;
		return *this;
	}

	inline float4x4& float4x4::operator -=(const float4x4& mat)
	{
		*this = *this - mat;
		return *this;
	}

	inline const float4x4 float4x4::operator -() const
	{
		return float4x4(-mCol0, -mCol1, -mCol2, -mCol3);
	}

	inline const float4x4 float4x4::operator *(float scalar) const
	{
		return float4x4(mCol0 * scalar, mCol1 * scalar, mCol2 * scalar, mCol3 * scalar);
	}

	inline float4x4& float4x4::operator *=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	inline const float4 float4x4::operator *(const float4& vec) const
	{
		return float4(
			((((mCol0.x * vec.x) + (mCol1.x * vec.y)) + (mCol2.x * vec.z)) + (mCol3.x * vec.w)),
			((((mCol0.y * vec.x) + (mCol1.y * vec.y)) + (mCol2.y * vec.z)) + (mCol3.y * vec.w)),
			((((mCol0.z * vec.x) + (mCol1.z * vec.y)) + (mCol2.z * vec.z)) + (mCol3.z * vec.w)),
			((((mCol0.w * vec.x) + (mCol1.w * vec.y)) + (mCol2.w * vec.z)) + (mCol3.w * vec.w))
			);
	}

	inline const float4 float4x4::operator *(const float3& vec) const
	{
		return float4(
			(((mCol0.x * vec.x) + (mCol1.x * vec.y)) + (mCol2.x * vec.z)),
			(((mCol0.y * vec.x) + (mCol1.y * vec.y)) + (mCol2.y * vec.z)),
			(((mCol0.z * vec.x) + (mCol1.z * vec.y)) + (mCol2.z * vec.z)),
			(((mCol0.w * vec.x) + (mCol1.w * vec.y)) + (mCol2.w * vec.z))
			);
	}

	inline const float4x4 float4x4::operator *(const float4x4& mat) const
	{
		return float4x4(
			(*this * mat.mCol0),
			(*this * mat.mCol1),
			(*this * mat.mCol2),
			(*this * mat.mCol3)
			);
	}

	inline float4x4& float4x4::operator *=(const float4x4& mat)
	{
		*this = *this * mat;
		return *this;
	}

	inline const float4x4 float4x4::identity()
	{
		return float4x4(
			float4(1.0f, 0.0f, 0.0f, 0.0f),
			float4(0.0f, 1.0f, 0.0f, 0.0f),
			float4(0.0f, 0.0f, 1.0f, 0.0f),
			float4(0.0f, 0.0f, 0.0f, 1.0f)
			);
	}

	inline float4x4& float4x4::setTranslation(const float3& translateVec)
	{
		mCol3.x = translateVec.x;
		mCol3.y = translateVec.y;
		mCol3.z = translateVec.z;
		return *this;
	}

	inline const float3 float4x4::getTranslation() const
	{
		return mCol3.XYZ();
	}
};


inline const float4x4 makeRotationMatrixX(float radians)
{
	float s, c;
	s = sinf(radians);
	c = cosf(radians);
	return float4x4(
		float4(1.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, c, s, 0.0f),
		float4(0.0f, -s, c, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
		);
}

inline const float4x4 makeRotationMatrixY(float radians)
{
	float s, c;
	s = sinf(radians);
	c = cosf(radians);
	return float4x4(
		float4(c, 0.0f, -s, 0.0f),
		float4(0.0f, 1.0f, 0.0f, 0.0f),
		float4(s, 0.0f, c, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
		);
}

inline const float4x4 makeRotationMatrixZ(float radians)
{
	float s, c;
	s = sinf(radians);
	c = cosf(radians);
	return float4x4(
		float4(c, s, 0.0f, 0.0f),
		float4(-s, c, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 1.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
		);
}

inline const float4x4 makeRotationMatrixXYZ(const float3& radiansXYZ)
{
	float sX, cX, sY, cY, sZ, cZ, tmp0, tmp1;
	sX = sinf(radiansXYZ.x);
	cX = cosf(radiansXYZ.x);
	sY = sinf(radiansXYZ.y);
	cY = cosf(radiansXYZ.y);
	sZ = sinf(radiansXYZ.z);
	cZ = cosf(radiansXYZ.z);
	tmp0 = (cZ * sY);
	tmp1 = (sZ * sY);
	return float4x4(
		float4((cZ * cY),					(sZ * cY),					-sY,		0.0f),
		float4(((tmp0 * sX) - (sZ * cX)),	((tmp1 * sX) + (cZ * cX)),	(cY * sX),	0.0f),
		float4(((tmp0 * cX) + (sZ * sX)),	((tmp1 * cX) - (cZ * sX)),	(cY * cX),	0.0f),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
		);
}

inline const float4x4 makeRotationMatrix(float radians, const float3& unitVec)
{
	float x, y, z, s, c, oneMinusC, xy, yz, zx;
	s = sinf(radians);
	c = cosf(radians);
	x = unitVec.x;
	y = unitVec.y;
	z = unitVec.z;
	xy = (x * y);
	yz = (y * z);
	zx = (z * x);
	oneMinusC = (1.0f - c);
	return float4x4(
		float4((((x * x) * oneMinusC) + c),		((xy * oneMinusC) + (z * s)),	((zx * oneMinusC) - (y * s)),	0.0f),
		float4(((xy * oneMinusC) - (z * s)),	(((y * y) * oneMinusC) + c),	((yz * oneMinusC) + (x * s)),	0.0f),
		float4(((zx * oneMinusC) + (y * s)),	((yz * oneMinusC) - (x * s)),	(((z * z) * oneMinusC) + c),	0.0f),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
		);
}

inline const float4x4 makeScaleMatrix(const float3& scaleVec)
{
	return float4x4(
		float4(scaleVec.x, 0.0f, 0.0f, 0.0f),
		float4(0.0f, scaleVec.y, 0.0f, 0.0f),
		float4(0.0f, 0.0f, scaleVec.z, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
		);
}

inline const float4x4 makeTranslationMatrix(const float3& translateVec)
{
	return float4x4(
		float4(1.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 1.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 1.0f, 0.0f),
		float4(translateVec, 1.0f)
		);
}

inline const float4x4 makeLookAtMatrix(const float3& eyePos, const float3& lookAtPos, const float3& upVec)
{
	float4x4 result;
	float3 camX, camY, camZ;
	camY = normalize(upVec);
	camZ = normalize((lookAtPos - eyePos));
	camX = normalize(cross(camY, camZ));
	camY = cross(camZ, camX);
	result = float4x4(float4(camX, 0.0), float4(camY, 0.0), float4(camZ, 0.0), float4(eyePos, 0.0));
	return orthoInverse(result);
}

inline const float4x4 makePerspectiveMatrix(float fovyRadians, float aspect, float zNear, float zFar)
{
	float f, rangeInv;
	f = tanf(((float)(1.570796327f) - (0.5f * fovyRadians)));
	rangeInv = (1.0f / (zFar - zNear));
	return float4x4(
		float4((f / aspect),	0.0f,	0.0f,							0.0f),
		float4(0.0f,			f,		0.0f,							0.0f),
		float4(0.0f,			0.0f,	zFar * rangeInv,				1.0f),
		float4(0.0f,			0.0f,	((-zNear * zFar) * rangeInv),	0.0f)
		);
}

inline const float4x4 makeFrustum(float left, float right, float bottom, float top, float zNear, float zFar)
{
	float sum_rl, sum_tb, sum_nf, inv_rl, inv_tb, inv_nf, n2;
	sum_rl = (right + left);
	sum_tb = (top + bottom);
	sum_nf = (zNear + zFar);
	inv_rl = (1.0f / (right - left));
	inv_tb = (1.0f / (top - bottom));
	inv_nf = (1.0f / (zNear - zFar));
	n2 = (zNear + zNear);
	return float4x4(
		float4((n2 * inv_rl), 0.0f, 0.0f, 0.0f),
		float4(0.0f, (n2 * inv_tb), 0.0f, 0.0f),
		float4((sum_rl * inv_rl), (sum_tb * inv_tb), (sum_nf * inv_nf), -1.0f),
		float4(0.0f, 0.0f, ((n2 * inv_nf) * zFar), 0.0f)
		);
}

inline const float4x4 makeOrthographicMatrix(float left, float right, float bottom, float top, float zNear, float zFar)
{
	float sum_rl, sum_tb, sum_nf, inv_rl, inv_tb, inv_nf;
	sum_rl = (right + left);
	sum_tb = (top + bottom);
	sum_nf = (zNear + zFar);
	inv_rl = (1.0f / (right - left));
	inv_tb = (1.0f / (top - bottom));
	inv_nf = (1.0f / (zNear - zFar));
	return float4x4(
		float4((inv_rl + inv_rl), 0.0f, 0.0f, 0.0f),
		float4(0.0f, (inv_tb + inv_tb), 0.0f, 0.0f),
		float4(0.0f, 0.0f, (inv_nf + inv_nf), 0.0f),
		float4((-sum_rl * inv_rl), (-sum_tb * inv_tb), (sum_nf * inv_nf), 1.0f)
		);
}

inline const float4x4 operator *(float scalar, const float4x4& mat)
{
	return mat * scalar;
}

inline const float4x4 outer(const float4& tfrm0, const float4& tfrm1)
{
	return float4x4(
		(tfrm0 * tfrm1.x),
		(tfrm0 * tfrm1.y),
		(tfrm0 * tfrm1.z),
		(tfrm0 * tfrm1.w)
		);
}

inline const float4x4 transpose(const float4x4& mat)
{
	return float4x4(
		float4(mat[0].x, mat[1].x, mat[2].x, mat[3].x),
		float4(mat[0].y, mat[1].y, mat[2].y, mat[3].y),
		float4(mat[0].z, mat[1].z, mat[2].z, mat[3].z),
		float4(mat[0].w, mat[1].w, mat[2].w, mat[3].w)
		);
}

inline const float4x4 orthoInverse(const float4x4 & tfrm)
{
	float3 inv0, inv1, inv2;
	inv0 = float3(tfrm[0].x, tfrm[1].x, tfrm[2].x);
	inv1 = float3(tfrm[0].y, tfrm[1].y, tfrm[2].y);
	inv2 = float3(tfrm[0].z, tfrm[1].z, tfrm[2].z);
	return float4x4(float4(inv0, 0.0), float4(inv1, 0.0), float4(inv2, 0.0),
		float4((-((inv0 * tfrm[3].x) + ((inv1 * tfrm[3].y) + (inv2 * tfrm[3].z)))), 1.0) );
}

inline const float4x4 inverse(const float4x4& mat)
{
	float4 res0, res1, res2, res3;
	float	m00, m01, m02, m03, 
			m10, m11, m12, m13, 
			m20, m21, m22, m23, 
			m30, m31, m32, m33, 
			tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, detInv;
	m00 = mat[0].x;	m01 = mat[0].y;	m02 = mat[0].z;	m03 = mat[0].w;
	m10 = mat[1].x;	m11 = mat[1].y;	m12 = mat[1].z;	m13 = mat[1].w;
	m20 = mat[2].x;	m21 = mat[2].y;	m22 = mat[2].z;	m23 = mat[2].w;
	m30 = mat[3].x;	m31 = mat[3].y;	m32 = mat[3].z;	m33 = mat[3].w;
	tmp0 = ((m22 * m03) - (m02 * m23));
	tmp1 = ((m32 * m13) - (m12 * m33));
	tmp2 = ((m01 * m22) - (m21 * m02));
	tmp3 = ((m11 * m32) - (m31 * m12));
	tmp4 = ((m21 * m03) - (m01 * m23));
	tmp5 = ((m31 * m13) - (m11 * m33));
	res0.x = ((((m21 * tmp1) - (m23 * tmp3)) - (m22 * tmp5)));
	res0.y = ((((m31 * tmp0) - (m33 * tmp2)) - (m32 * tmp4)));
	res0.z = ((((m03 * tmp3) + (m02 * tmp5)) - (m01 * tmp1)));
	res0.w = ((((m13 * tmp2) + (m12 * tmp4)) - (m11 * tmp0)));
	detInv = (1.0f / ((((m00 * res0.x) + (m10 * res0.y)) + (m20 * res0.z)) + (m30 * res0.w)));
	res1.x = ((m20 * tmp1));
	res1.y = ((m30 * tmp0));
	res1.z = ((m00 * tmp1));
	res1.w = ((m10 * tmp0));
	res3.x = ((m20 * tmp3));
	res3.y = ((m30 * tmp2));
	res3.z = ((m00 * tmp3));
	res3.w = ((m10 * tmp2));
	res2.x = ((m20 * tmp5));
	res2.y = ((m30 * tmp4));
	res2.z = ((m00 * tmp5));
	res2.w = ((m10 * tmp4));
	tmp0 = ((m20 * m01) - (m00 * m21));
	tmp1 = ((m30 * m11) - (m10 * m31));
	tmp2 = ((m20 * m03) - (m00 * m23));
	tmp3 = ((m30 * m13) - (m10 * m33));
	tmp4 = ((m20 * m02) - (m00 * m22));
	tmp5 = ((m30 * m12) - (m10 * m32));
	res2.x = ((((m23 * tmp1) - (m21 * tmp3)) + res2.x));
	res2.y = ((((m33 * tmp0) - (m31 * tmp2)) + res2.y));
	res2.z = ((((m01 * tmp3) - (m03 * tmp1)) - res2.z));
	res2.w = ((((m11 * tmp2) - (m13 * tmp0)) - res2.w));
	res3.x = ((((m21 * tmp5) - (m22 * tmp1)) + res3.x));
	res3.y = ((((m31 * tmp4) - (m32 * tmp0)) + res3.y));
	res3.z = ((((m02 * tmp1) - (m01 * tmp5)) - res3.z));
	res3.w = ((((m12 * tmp0) - (m11 * tmp4)) - res3.w));
	res1.x = ((((m22 * tmp3) - (m23 * tmp5)) - res1.x));
	res1.y = ((((m32 * tmp2) - (m33 * tmp4)) - res1.y));
	res1.z = ((((m03 * tmp5) - (m02 * tmp3)) + res1.z));
	res1.w = ((((m13 * tmp4) - (m12 * tmp2)) + res1.w));
	return float4x4((res0 * detInv), (res1 * detInv), (res2 * detInv), (res3 * detInv));
}

inline float determinant(const float4x4& mat)
{
	float dx, dy, dz, dw, 
		m00, m01, m02, m03, 
		m10, m11, m12, m13, 
		m20, m21, m22, m23, 
		m30, m31, m32, m33, 
		tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;
	m00 = mat[0].x;	m01 = mat[0].y;	m02 = mat[0].z;	m03 = mat[0].w;
	m10 = mat[1].x;	m11 = mat[1].y;	m12 = mat[1].z;	m13 = mat[1].w;
	m20 = mat[2].x;	m21 = mat[2].y;	m22 = mat[2].z;	m23 = mat[2].w;
	m30 = mat[3].x;	m31 = mat[3].y;	m32 = mat[3].z;	m33 = mat[3].w;
	tmp0 = ((m22 * m03) - (m02 * m23));
	tmp1 = ((m32 * m13) - (m12 * m33));
	tmp2 = ((m01 * m22) - (m21 * m02));
	tmp3 = ((m11 * m32) - (m31 * m12));
	tmp4 = ((m21 * m03) - (m01 * m23));
	tmp5 = ((m31 * m13) - (m11 * m33));
	dx = (((m21 * tmp1) - (m23 * tmp3)) - (m22 * tmp5));
	dy = (((m31 * tmp0) - (m33 * tmp2)) - (m32 * tmp4));
	dz = (((m03 * tmp3) + (m02 * tmp5)) - (m01 * tmp1));
	dw = (((m13 * tmp2) + (m12 * tmp4)) - (m11 * tmp0));
	return ((((m00 * dx) + (m10 * dy)) + (m20 * dz)) + (m30 * dw));
}
