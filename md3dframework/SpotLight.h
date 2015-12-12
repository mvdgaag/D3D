#pragma once
#include "GaagCommon.h"
#include "BaseResource.h"

REGISTERCLASS(SpotLight);


class SpotLight : public BaseResource
{
public:
	SpotLight() {}
	~SpotLight() {}

	void Init(float3 inPosition, float inRadius, float3 inDirection, float inConeCosine, float4 inColor) 
	{ 
		SetPosition(inPosition);
		SetRadius(inRadius);
		SetDirection(inDirection);
		SetConeCosine(inConeCosine);
		SetColor(inColor);
	}
	void CleanUp() override {}

	float3	GetPosition()						{ return float3(mPosition); }
	float3	GetDirection()						{ return float3(mDirection); }
	float	GetRadius()							{ return mPosition.w; }
	float	GetAngle()							{ return acos(mDirection.w) * 2.0f; }
	float	GetConeCosine()						{ return mDirection.w; }
	float4	GetColor()							{ return mColor; }

	void	SetPosition(float3 inPosition)		{ mPosition = float4(inPosition, mPosition.w); }
	void	SetDirection(float3 inDirection)	{ mDirection = float4(normalize(inDirection), mDirection.w); }
	void	SetRadius(float inRadius)			{ mPosition.w = inRadius; }
	void	SetConeCosine(float inCosine)		{ mDirection.w = inCosine; }
	void	SetAngle(float inAngle)				{ mDirection.w = cos(inAngle / 2.0f); }
	void	SetColor(float4 inColor)			{ mColor = inColor; }

	void	LookAt(float3 inLocation)			{ SetDirection(inLocation - float3(mPosition)); }

	ResourceType GetResourceType() const override { return ResourceType::SPOT_LIGHT; }

protected:
	float4	mPosition;	// x, y, z, range
	float4	mDirection; // x, y, z, cone cosine
	float4	mColor;		// r, g, b, a

private:
	SpotLight(SpotLight const&) = delete;
	void operator=(SpotLight const&) = delete;
};

