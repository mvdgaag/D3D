#include "Frustum.h"


void Frustum::InitFromProjectionMatrix(float4x4 inProjectionMatrix)
{
	// left
	mPlanes[0].x = inProjectionMatrix[0][3] + inProjectionMatrix[0][0];
	mPlanes[0].y = inProjectionMatrix[1][3] + inProjectionMatrix[1][0];
	mPlanes[0].z = inProjectionMatrix[2][3] + inProjectionMatrix[2][0];
	mPlanes[0].w = inProjectionMatrix[3][3] + inProjectionMatrix[3][0];

	// right
	mPlanes[1].x = inProjectionMatrix[0][3] - inProjectionMatrix[0][0];
	mPlanes[1].y = inProjectionMatrix[1][3] - inProjectionMatrix[1][0];
	mPlanes[1].z = inProjectionMatrix[2][3] - inProjectionMatrix[2][0];
	mPlanes[1].w = inProjectionMatrix[3][3] - inProjectionMatrix[3][0];

	// bottom
	mPlanes[2].x = inProjectionMatrix[0][3] + inProjectionMatrix[0][1];
	mPlanes[2].y = inProjectionMatrix[1][3] + inProjectionMatrix[1][1];
	mPlanes[2].z = inProjectionMatrix[2][3] + inProjectionMatrix[2][1];
	mPlanes[2].w = inProjectionMatrix[3][3] + inProjectionMatrix[3][1];

	// top
	mPlanes[3].x = inProjectionMatrix[0][3] - inProjectionMatrix[0][1];
	mPlanes[3].y = inProjectionMatrix[1][3] - inProjectionMatrix[1][1];
	mPlanes[3].z = inProjectionMatrix[2][3] - inProjectionMatrix[2][1];
	mPlanes[3].w = inProjectionMatrix[3][3] - inProjectionMatrix[3][1];

	// near
	mPlanes[4].x = inProjectionMatrix[0][3] + inProjectionMatrix[0][2];
	mPlanes[4].y = inProjectionMatrix[1][3] + inProjectionMatrix[1][2];
	mPlanes[4].z = inProjectionMatrix[2][3] + inProjectionMatrix[2][2];
	mPlanes[4].w = inProjectionMatrix[3][3] + inProjectionMatrix[3][2];

	// far
	mPlanes[5].x = inProjectionMatrix[0][3] - inProjectionMatrix[0][2];
	mPlanes[5].y = inProjectionMatrix[1][3] - inProjectionMatrix[1][2];
	mPlanes[5].z = inProjectionMatrix[2][3] - inProjectionMatrix[2][2];
	mPlanes[5].w = inProjectionMatrix[3][3] - inProjectionMatrix[3][2];

	for (int i = 0; i < 6; i++)
		normalize(mPlanes[i]);
}


bool Frustum::TestPoint(float3 inPosition)
{
	return TestSphere(inPosition, 0.0);
}


bool Frustum::TestBox(float3 inCenter, float3 inSize)
{
	// todo
	return true;
}


bool Frustum::TestSphere(float3 inCenter, float inRadius)
{
	for (int i = 0; i < 6; i++)
	{
		if (dot(mPlanes[i], float4(inCenter, 1.0)) < -inRadius)
			return false;
	}
	return true;
}