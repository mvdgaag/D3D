#include "Camera.h"
#include "RenderContext.h"


Camera::Camera()
{
	mPosition = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0);
	mTarget = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0);
	mUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0);
	SetProjectionMatrix(0.1, 100.0, DirectX::XM_PIDIV4);
}


Camera::~Camera()
{
}


DirectX::XMMATRIX Camera::GetProjectionMatrix()
{
	return mProjection;
}


DirectX::XMMATRIX Camera::GetViewMatrix()
{
	return DirectX::XMMatrixLookAtLH(mPosition, mTarget, mUp);
}


void Camera::SetProjectionMatrix(float inNear, float inFar, float inFovY)
{
	mAspect = float(theRenderContext.GetWidth()) / theRenderContext.GetHeight();
	mFovY = inFovY;
	mNear = inNear;
	mFar = inFar;
	mProjection = DirectX::XMMatrixPerspectiveFovLH(mFovY, mAspect, mNear, mFar);
}


DirectX::XMMATRIX Camera::GetViewProjectionMatrix()
{
	return GetViewMatrix() * GetProjectionMatrix();
}