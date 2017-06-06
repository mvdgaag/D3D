#include "Brush.h"


void Brush::Init(pComputeShader inShader, float inRadius, float inFalloff, float inStrength, bool inSamplesNeighbors)
{
	SetShader(inShader);
	SetRadius(inRadius);
	SetFalloffFraction(inFalloff);
	SetStrength(inStrength);
	mSamplesNeighbors = inSamplesNeighbors;
}


void Brush::Apply(pRenderTarget inTarget, const rect& inPixelRect, const float2& inWorldPosition, const apTexture inNeighbors)
{
	assert(inTarget != nullptr);

	int2 resolution = inTarget->GetDimensions();
	if (inPixelRect.GetArea() <= 0)
		return;

	mConstantBuffer.rect = int4(inPixelRect.topLeft, inPixelRect.bottomRight);
	mConstantBuffer.texInfo = int4(inTarget->GetDimensions(), 0, 0);
	mConstantBuffer.paintData = float4(mStrength, mFalloffFraction, inWorldPosition);

	pConstantBuffer cbuf = theResourceFactory.MakeConstantBuffer(sizeof(mConstantBuffer));
	theRenderContext.UpdateSubResource(*cbuf, &mConstantBuffer);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetConstantBuffer(cbuf, 0);

	if (mSamplesNeighbors)
	{
		assert(inNeighbors.size() == 3);
		if (inNeighbors[0] != nullptr) theRenderContext.CSSetTexture(inNeighbors[0], 0);
		if (inNeighbors[1] != nullptr) theRenderContext.CSSetTexture(inNeighbors[1], 1);
		if (inNeighbors[2] != nullptr) theRenderContext.CSSetTexture(inNeighbors[2], 2);
	}

	theRenderContext.CSSetRWTexture(inTarget, 0, 0);

	int2 num_threads = ((inPixelRect.bottomRight - inPixelRect.topLeft) + 7) / 8;
	theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

	if (mSamplesNeighbors)
	{
		theRenderContext.CSSetTexture(NULL, 0);
		theRenderContext.CSSetTexture(NULL, 1);
		theRenderContext.CSSetTexture(NULL, 2);
	}
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetShader(NULL);
}