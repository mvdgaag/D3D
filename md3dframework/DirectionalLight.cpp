#include "DirectionalLight.h"
#include "GaagCommon.h"
#include "ResourceFactory.h"


void DirectionalLight::Init(float3 inDirection, float4 inColor, int inShadowMapSize)
{
	SetDirection(inDirection);
	SetColor(inColor);
	if (inShadowMapSize > 0)
	{
		int2 size = int2(inShadowMapSize, inShadowMapSize);
		mDepthStencilTarget = theResourceFactory.MakeDepthStencilTarget(size);
		mShadowMap = theResourceFactory.MakeRenderTarget(size, 1, FORMAT_R32_FLOAT);
	}
}


void DirectionalLight::CleanUp() 
{
	if (mShadowMap)
	{
		theResourceFactory.DestroyItem(mShadowMap);
		mShadowMap = nullptr;
	}
	if (mDepthStencilTarget)
	{
		theResourceFactory.DestroyItem(mDepthStencilTarget);
		mDepthStencilTarget = nullptr;
	}
}
