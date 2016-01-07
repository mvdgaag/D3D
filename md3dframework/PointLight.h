#pragma once
#include "GaagCommon.h"
#include "BaseResource.h"

REGISTERCLASS(PointLight);


class PointLight : public BaseResource
{
public:
	PointLight() {}
	~PointLight() { CleanUp(); }

	void Init(float3 inPosition, float inRadius, float4 inColor) { mPosition = float4(inPosition, inRadius); mColor = inColor; }
	void CleanUp() override {}

	float3	GetPosition()					{ return float3(mPosition); }
	float	GetRadius()						{ return mPosition.w; }
	float4	GetColor()						{ return mColor; }

	void	SetPosition(float3 inPosition)	{ mPosition = float4(inPosition, mPosition.w); }
	void	SetRadius(float inRadius)		{ mPosition.w = inRadius; }
	void	SetColor(float4 inColor)		{ mColor = inColor; }

	ResourceType GetResourceType() const override { return ResourceType::POINT_LIGHT; }

protected:
	float4	mPosition;	// x, y, z, radius
	float4	mColor;		// r, g, b, a

private:
	PointLight(PointLight const&) = delete;
	void operator=(PointLight const&) = delete;
};

