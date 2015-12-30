#pragma once
#include "GaagCommon.h"


class BaseDrawable
{
public:
	explicit BaseDrawable() {}
	virtual ~BaseDrawable() {}
	
	// TODO: test
	const float3				GetPosition()		{ return float3(mTransform[3]) / mTransform[3].w; }
	//

	const float4x4&				GetTransform()		{ return mTransform; }
	const float4x4&				GetPrevTransform()	{ return mPrevTransform; }
	void						SwapTransform()		{ mPrevTransform = mTransform; }
	
	void Rotate(float3 inAxis, float inAngle)	
	{ 
		mTransform = rotate(mTransform, inAngle, inAxis);
	}
	
	void Translate(float3 inTranslation)			
	{ 
		mTransform = translate(mTransform, inTranslation);
	}

protected:
	float4x4 mTransform;
	float4x4 mPrevTransform;
};

