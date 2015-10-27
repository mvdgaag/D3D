#pragma once
#include "GAAGCommon.h"


class BaseDrawable
{
public:
	explicit BaseDrawable() {}
	virtual ~BaseDrawable() {}
	
	void Rotate(float3 inAxis, float inAngle) { mTransform *= DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(inAxis.x, inAxis.y, inAxis.z, 0.0), inAngle); }
	void Translate(float3 inTranslation) { mTransform *= DirectX::XMMatrixTranslation(inTranslation.x, inTranslation.y, inTranslation.z); }

protected:
	DirectX::XMMATRIX mTransform;
};

