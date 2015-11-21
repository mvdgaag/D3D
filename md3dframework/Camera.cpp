#include "Camera.h"
#include "RenderContext.h"


Camera::Camera()
{
	mPosition = float3(0.0f, 0.0f, -5.0f);
	mTarget = float3(0.0f, 0.0f, 0.0f);
	mWorldUp = float3(0.0f, 1.0f, 0.0f);
	SetProjectionMatrix(0.1, 1000.0, 3.1415 / 4.0);
}


Camera::~Camera()
{
}


void Camera::SetProjectionMatrix(float inNear, float inFar, float inFovY)
{
	mAspect = float(theRenderContext.GetWidth()) / theRenderContext.GetHeight();
	mFovY = inFovY;
	mNear = inNear;
	mFar = inFar;
}