#pragma once
#include "Gaag.h"

REGISTERCLASS(Brush);

class Brush
{
public:
	Brush() {};
	~Brush() {};
	
	void			Init(pComputeShader inShader, float inRadius = 25.0, float inFalloff = 0.5, float inStrength = 0.1, bool inSamplesNeighbors = false);
	void			Apply(pRenderTarget inTile, const rect& inPixelRect, const float2& inWorldPosition, const apTexture inNeighbors);
	
	void			SetRadius(float inRadius)					{ mRadius = inRadius; }
	float			GetRadius()									{ return mRadius; }
	void			SetShader(pComputeShader inShader)			{ mShader = inShader; }
	pComputeShader	GetShader()									{ return mShader; }
	void			SetFalloffFraction(float inFalloffFraction) { mFalloffFraction = inFalloffFraction; }
	float			GetFalloffFraction()						{ return mFalloffFraction; }
	void			SetStrength(float inStrength)				{ mStrength = inStrength; }
	float			GetStrength()								{ return mStrength; }
	bool			SamplesNeighbors()							{ return mSamplesNeighbors; }
private:
	struct ConstantBufferData
	{
		int4	rect;
		int4	texInfo;
		float4	paintData;
	};
	ConstantBufferData mConstantBuffer;
	float mRadius;
	float mFalloffFraction;
	float mStrength;
	bool mSamplesNeighbors;
	pComputeShader mShader;
};

