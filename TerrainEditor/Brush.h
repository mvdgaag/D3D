#pragma once
#include "Gaag.h"
#include "TerrainTile.h"

PREDEFINE(Brush, pBrush);

class Brush
{
public:
	Brush() {};
	~Brush() {};
	
	void Apply(pTerrainTile inTile, const rect& inPixelRect);
	
	void			SetRadius(float inRadius)			{ mRadius = inRadius; }
	float			GetRadius()							{ return mRadius; }
	void			SetShader(pComputeShader inShader)	{ mShader = inShader; }
	pComputeShader	GetShader()							{ return mShader; }
private:
	struct ConstantBufferData
	{
		float4 rect;
		float2 coord;
		float2 radius;
		int2 resolution;
		int2 bogus;
	};
	ConstantBufferData mConstantBuffer;
	float mRadius;
	pComputeShader mShader;
};

