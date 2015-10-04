#include "Camera.h"
#include "main.h"
#include "Framework.h"


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


float halton23x[8] = {	1.0f/2.0f, 1.0f/4.0f, 3.0f/4.0f, 1.0f/8.0f, 
						5.0f/8.0f, 3.0f/8.0f, 7.0f/8.0f, 1.0f/16.0f };
float halton23y[8] = {	1.0f/3.0f, 2.0f/3.0f, 1.0f/9.0f, 4.0f/9.0f,
						7.0f/9.0f, 2.0f/9.0f, 5.0f/9.0f, 8.0f/9.0f };


DirectX::XMMATRIX Camera::GetProjectionMatrix()
{
	//return mProjection;
	static DirectX::XMMATRIX jittered_matrix;
	jittered_matrix = mProjection;
	int idx = theFramework.GetFrameID() % 8;
	jittered_matrix(0, 2) += (halton23x[idx] / 800.0f);
	jittered_matrix(1, 2) += (halton23y[idx] / 600.0f);
	return jittered_matrix;
}


DirectX::XMMATRIX Camera::GetViewMatrix()
{
	return DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(mPosition, mTarget, mUp));
}


void Camera::SetProjectionMatrix(float inNear, float inFar, float inFovY)
{
	float aspect = float(theFramework.GetScreenWidth()) / theFramework.GetScreenHeight();
	mProjection = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(inFovY, aspect, inNear, inFar));
}


DirectX::XMMATRIX Camera::GetViewProjectionMatrix()
{
	// TODO: doesn't work, multiply before transposes?
	return GetViewMatrix() * GetProjectionMatrix();
}