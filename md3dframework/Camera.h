#pragma once
#include "GaagCommon.h"

REGISTERCLASS(Camera)

class Camera
{
public:
	Camera();
	~Camera();

	float4x4	GetViewMatrix() { return lookAt(mPosition, mTarget, mWorldUp); }
	float4x4	GetProjectionMatrix() { return perspective(mFovY, mAspect, mNear, mFar); }
	float4x4	GetViewProjectionMatrix() { return GetProjectionMatrix() * GetViewMatrix(); };
	Frustum		ExtractFrustum() { Frustum result; result.InitFromProjectionMatrix(GetViewProjectionMatrix()); return result; }

	float		GetFovX() { return mFovY * mAspect; }
	float		GetFovY() { return mFovY; }
	float		GetAspect() { return mAspect; }
	float		GetNear() { return mNear; }
	float		GetFar() { return mFar; }
	float3		GetPosition() { return mPosition; }

	// TODO: test, need to transpose?
	float3		GetRight()		{ float4x4 view_mat = GetViewMatrix(); return float3(view_mat[0].x, view_mat[1].x, view_mat[2].x); }
	float3		GetUp()			{ float4x4 view_mat = GetViewMatrix(); return float3(view_mat[0].y, view_mat[1].y, view_mat[2].y); }
	float3		GetForward()	{ float4x4 view_mat = GetViewMatrix(); return float3(view_mat[0].z, view_mat[1].z, view_mat[2].z); }

	void		SetPosition(float inX, float inY, float inZ) { SetPosition(float3(inX, inY, inZ)); }
	void		SetPosition(float3 inPosition) { mPosition = inPosition; }
	void		SetTarget(float inX, float inY, float inZ) { SetTarget(float3(inX, inY, inZ)); }
	void		SetTarget(float3 inTarget) { mTarget = inTarget; }
	void		SetWorldUp(float inX, float inY, float inZ) { SetWorldUp(float3(inX, inY, inZ)); }
	void		SetWorldUp(float3 inUp) { mWorldUp = inUp; }
	void		SetProjectionMatrix(float inNear, float inFar, float inFovY);

private:
	float3 mPosition;
	float3 mTarget;
	float3 mWorldUp;

	float mFovY;
	float mNear;
	float mAspect;
	float mFar;
};

