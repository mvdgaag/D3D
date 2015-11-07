#pragma once
#include "GaagCommon.h"

PREDEFINE(Camera, pCamera)

class Camera
{
public:
	Camera();
	~Camera();

	float4x4			GetViewMatrix() { return lookAt(mPosition, mTarget, mUp); }
	float4x4			GetProjectionMatrix() { return perspective(mFovY, mAspect, mNear, mFar); }
	float4x4			GetViewProjectionMatrix() { return GetProjectionMatrix() * GetViewMatrix(); };

	float		GetFovX() { return mFovY * mAspect; }
	float		GetFovY() { return mFovY; }
	float		GetAspect() { return mAspect; }
	float		GetNear() { return mNear; }
	float		GetFar() { return mFar; }
	float3		GetPosition() { return mPosition; }

	void		SetPosition(float x, float y, float z) { mPosition = float3(x, y, z); }
	void		SetTarget(float x, float y, float z) { mTarget = float3(x, y, z); }
	void		SetUp(float x, float y, float z) { mUp = float3(x, y, z); }
	void		SetProjectionMatrix(float inNear, float inFar, float inFovY);

private:
	float3 mPosition;
	float3 mTarget;
	float3 mUp;

	float mFovY;
	float mNear;
	float mAspect;
	float mFar;
};

