#pragma once
#include "GaagCommon.h"

PREDEFINE(Camera, pCamera)

class Camera
{
public:
	Camera();
	~Camera();

	DirectX::XMMATRIX	GetViewMatrix();
	DirectX::XMMATRIX	GetProjectionMatrix();
	DirectX::XMMATRIX	GetViewProjectionMatrix();

	float		GetFovX() { return mFovY * mAspect; }
	float		GetFovY() { return mFovY; }
	float		GetAspect() { return mAspect; }
	float		GetNear() { return mNear; }
	float		GetFar() { return mFar; }
	float3		GetPosition() { return float3(mPosition.m128_f32[0], mPosition.m128_f32[1], mPosition.m128_f32[2]); }

	void		SetPosition(DirectX::XMVECTOR inPosition) { mPosition = inPosition; }
	void		SetPosition(float x, float y, float z) { mPosition = DirectX::XMVectorSet(x, y, z, 0.0f); }
	void		SetTarget(DirectX::XMVECTOR inTarget) { mTarget = inTarget; }
	void		SetTarget(float x, float y, float z) { mTarget = DirectX::XMVectorSet(x, y, z, 0.0f); }
	void		SetUp(DirectX::XMVECTOR inUp) { mUp = inUp; }
	void		SetUp(float x, float y, float z) { mUp = DirectX::XMVectorSet(x, y, z, 0.0f); }
	void		SetProjectionMatrix(float inNear, float inFar, float inFovY);

private:
	DirectX::XMMATRIX mProjection;
	DirectX::XMMATRIX mView;

	DirectX::XMVECTOR mPosition;
	DirectX::XMVECTOR mTarget;
	DirectX::XMVECTOR mUp;

	float mFovY;
	float mNear;
	float mAspect;
	float mFar;
};

