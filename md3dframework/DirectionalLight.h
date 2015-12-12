#pragma once
#include "GaagCommon.h"
#include "BaseResource.h"

REGISTERCLASS(DirectionalLight);


class DirectionalLight : public BaseResource
{
public:
	DirectionalLight() {}
	~DirectionalLight() {}

	void Init(float3 inDirection, float4 inColor) { SetDirection(inDirection); SetColor(inColor); }
	void CleanUp() override {}

	float3	GetDirection()						{ return float3(mDirection); }
	float4	GetColor()							{ return mColor; }

	void	SetDirection(float3 inDirection)	{ mDirection = float4(normalize(inDirection), 0.0); }
	void	SetColor(float4 inColor)			{ mColor = inColor; }

	ResourceType GetResourceType() const override { return ResourceType::DIRECTIONAL_LIGHT; }

protected:
	float4	mDirection;	// x, y, z, w
	float4	mColor;		// r, g, b, a

private:
	DirectionalLight(DirectionalLight const&) = delete;
	void operator=(DirectionalLight const&) = delete;
};

