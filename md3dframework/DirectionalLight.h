#pragma once
#include "GaagCommon.h"
#include "BaseResource.h"

REGISTERCLASS(DirectionalLight);
REGISTERCLASS(RenderTarget);
REGISTERCLASS(DepthStencilTarget);


class DirectionalLight : public BaseResource
{
public:
	DirectionalLight() {}
	~DirectionalLight() { CleanUp(); }

	void Init(float3 inDirection, float4 inColor, int inShadowMapSize = 0);
	void CleanUp() override;

	float3	GetDirection()						{ return float3(mDirection); }
	float4	GetColor()							{ return mColor; } // note that alpha component is a boolean for the shadow map
	pRenderTarget GetShadowMap()				{ return mShadowMap; }
	pDepthStencilTarget GetDepthStencilTarget()	{ return mDepthStencilTarget; }
	float4x4& GetShadowMatrix()					{ return mShadowMatrix; }

	void	SetDirection(float3 inDirection)			{ mDirection = float4(normalize(inDirection), 0.0); }
	void	SetColor(float4 inColor)					{ mColor = inColor; }

	ResourceType GetResourceType() const override { return ResourceType::DIRECTIONAL_LIGHT; }

protected:
	float4	mDirection;	// x, y, z, w
	float4	mColor;		// r, g, b, a

private:
	DirectionalLight(DirectionalLight const&) = delete;
	void operator=(DirectionalLight const&) = delete;
	
	pRenderTarget mShadowMap;
	pDepthStencilTarget mDepthStencilTarget;
	float4x4 mShadowMatrix;
};

