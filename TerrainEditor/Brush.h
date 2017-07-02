#pragma once
#include "Gaag.h"

REGISTERCLASS(Brush);

class Brush
{
public:
	Brush() {};
	~Brush() {};
	
	void			Init(pComputeShader inShader, float inRadius = 25.0, float inFalloff = 0.5, float4 inColor = float4(1.0, 1.0, 1.0, 1.0), float inStrength = 0.1, bool inSamplesNeighbors = false);
	void			Apply(pRenderTarget inTarget, const rect& inPixelRect, const rect& inWorldTileRect, const float2& inWorldPosition, const apTexture inNeighbors);
	
	void			SetRadius(float inRadius)					{ mRadius = inRadius; }
	float			GetRadius()									{ return mRadius; }
	void			SetShader(pComputeShader inShader)			{ mShader = inShader; }
	pComputeShader	GetShader()									{ return mShader; }
	void			SetFalloffFraction(float inFalloffFraction) { mFalloffFraction = inFalloffFraction; }
	float			GetFalloffFraction()						{ return mFalloffFraction; }
	void			SetColor(float4 inColor)					{ mColor = inColor; }
	float4			GetColor()									{ return mColor; }
	void			SetStrength(float inStrength)				{ mStrength = inStrength; }
	float			GetStrength()								{ return mStrength; }
	bool			SamplesNeighbors()							{ return mSamplesNeighbors; }
private:
	struct ConstantBufferData
	{
		int4	rect;
		int4	texInfo;
		float4	paintData;
		float4	brushData;
		float4  color;
		float4	worldTileRect;
	};

	ConstantBufferData mConstantBuffer;
	float mRadius;
	float mFalloffFraction;
	float4 mColor;
	float mStrength;
	bool mSamplesNeighbors;
	pComputeShader mShader;
};

