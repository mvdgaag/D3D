#pragma once
#include "GaagMath.h"


class Frustum
{
public:
	Frustum() {};
	~Frustum() {};

	void InitFromProjectionMatrix(float4x4 inProjectionMatrix);

	bool TestPoint(float3 inPosition);
	bool TestBox(float3 inCenter, float3 inSize);
	bool TestSphere(float3 inCenter, float inRadius);
private:
	float4 mPlanes[6];
};

