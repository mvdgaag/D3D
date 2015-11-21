#include "CameraController.h"
#include "Camera.h"

void CameraController::OnKeyDown(unsigned int inKey)
{
	float3 cam_pos = mTargetCamera->GetPosition();
	float3 cam_forward = mTargetCamera->GetForward();
	float3 cam_right = mTargetCamera->GetRight();
	float3 cam_up = mTargetCamera->GetUp();
	float3 cam_target = cam_pos - cam_forward;

	switch (inKey)
	{
	case 0x57: // w
		mTargetCamera->SetPosition(cam_pos - cam_forward);
		mTargetCamera->SetTarget(cam_target - cam_forward);
		break;
	case 0x53: // s
		mTargetCamera->SetPosition(cam_pos + cam_forward);
		mTargetCamera->SetTarget(cam_target + cam_forward);
		break;
	case 0x41: // a
		mTargetCamera->SetPosition(cam_pos - cam_right);
		mTargetCamera->SetTarget(cam_target - cam_right);
		break;
	case 0x44: // d
		mTargetCamera->SetPosition(cam_pos + cam_right);
		mTargetCamera->SetTarget(cam_target + cam_right);
		break;
	case 0x45: // e
		mTargetCamera->SetPosition(cam_pos + cam_up);
		mTargetCamera->SetTarget(cam_target + cam_up);
		break;
	case 0x5A: // z
		mTargetCamera->SetPosition(cam_pos - cam_up);
		mTargetCamera->SetTarget(cam_target - cam_up);
		break;
	case 0x25: // left
		mTargetCamera->SetPosition(cam_pos);
		mTargetCamera->SetTarget(cam_target - 0.05f * cam_right);
		break;
	case 0x27: // right
		mTargetCamera->SetPosition(cam_pos);
		mTargetCamera->SetTarget(cam_target + 0.05f * cam_right);
		break;
	case 0x26: // up
		mTargetCamera->SetPosition(cam_pos);
		mTargetCamera->SetTarget(cam_target + 0.05f * cam_up);
		break;
	case 0x28: // down
		mTargetCamera->SetPosition(cam_pos);
		mTargetCamera->SetTarget(cam_target - 0.05f * cam_up);
		break;
	}
}
