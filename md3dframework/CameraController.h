#pragma once
#include "GaagCommon.h"
#include "InputListener.h"

REGISTERCLASS(CameraController);
REGISTERCLASS(Camera);

class CameraController : public InputListener
{
public:
	CameraController() {}
	~CameraController() {}

	void	SetTargetCamera(pCamera inCamera)	{ mTargetCamera = inCamera; }
	pCamera	GetTargetCamera()					{ return mTargetCamera; }

	void OnKeyDown(unsigned int inKey) override;

private:
	pCamera mTargetCamera;
};

