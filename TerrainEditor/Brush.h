#pragma once
#include "Gaag.h"
#include "TerrainTile.h"

PREDEFINE(Brush, pBrush);

class Brush
{
public:
	Brush() {};
	~Brush() {};
	
	void			Init(pComputeShader inShader, float inRadius = 10.0, float inFalloff = 0.5, float inStrength = 1.0);
	void			Apply(pTerrainTile inTile, const rect& inPixelRect, const float2& inWorldPosition);
	
	void			SetRadius(float inRadius)					{ mRadius = inRadius; }
	float			GetRadius()									{ return mRadius; }
	void			SetShader(pComputeShader inShader)			{ mShader = inShader; }
	pComputeShader	GetShader()									{ return mShader; }
	void			SetFalloffFraction(float inFalloffFraction) { mFalloffFraction = inFalloffFraction; }
	float			GetFalloffFraction()						{ return mFalloffFraction; }
	void			SetStrength(float inStrength)				{ mStrength = inStrength; }
	float			GetStrength()								{ return mStrength; }
private:
	struct ConstantBufferData
	{
		int4	rect;
		float4	paintData;
		float4	brushData;
	};
	ConstantBufferData mConstantBuffer;
	float mRadius;
	float mFalloffFraction;
	float mStrength;
	pComputeShader mShader;
};

