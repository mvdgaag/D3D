#include "Brush.h"


void Brush::Init(pComputeShader inShader, float inRadius, float inFalloff, float inStrength)
{
	SetShader(inShader);
	SetRadius(inRadius);
	SetFalloffFraction(inFalloff);
	SetStrength(inStrength);
}


void Brush::Apply(pHeightFieldTile inTile, const rect& inPixelRect, const float2& inWorldPosition)
{
	int2 resolution = inTile->GetTexture()->GetResolution();
	if (inPixelRect.GetArea() <= 0)
		return;

	mConstantBuffer.rect = int4(inPixelRect.topLeft, inPixelRect.bottomRight);
	mConstantBuffer.paintData = float4(mStrength, mFalloffFraction, inWorldPosition);

	pConstantBuffer cbuf = MAKE_NEW(ConstantBuffer);
	cbuf->Init(sizeof(mConstantBuffer));
	theRenderContext.UpdateSubResource(*cbuf, &mConstantBuffer);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetConstantBuffer(cbuf, 0);
	if (inTile != nullptr) theRenderContext.CSSetRWTexture(inTile->GetRenderTarget(), 0);

	int2 num_threads = ((inPixelRect.bottomRight - inPixelRect.topLeft) + 7) / 8;
	theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetShader(NULL);
}