#include "Brush.h"


void Brush::Init(pComputeShader inShader, float inRadius, float inFalloff, float4 inColor, float inStrength, bool inSamplesNeighbors)
{
	SetShader(inShader);
	SetRadius(inRadius);
	SetFalloffFraction(inFalloff);
	SetColor(inColor);
	SetStrength(inStrength);
	mSamplesNeighbors = inSamplesNeighbors;
}


void Brush::Apply(pRenderTarget inTarget, const rect& inPixelRect, const rect& inWorldTileRect, const float2& inWorldPosition, const apTexture inNeighborhood)
{
	assert(inTarget != nullptr);
	assert(inNeighborhood.size() > 0);

	int2 resolution = inTarget->GetDimensions();
	if (inPixelRect.GetArea() <= 0)
		return;

	pRenderTarget rt = theResourceFactory.MakeRenderTarget(resolution, 1, inTarget->GetTexture()->GetFormat());
	TextureUtil::TextureCopy(rt, inNeighborhood[0]);

	mConstantBuffer.rect = int4(inPixelRect.topLeft, inPixelRect.bottomRight);
	mConstantBuffer.texInfo = int4(inTarget->GetDimensions(), 0, 0);
	mConstantBuffer.paintData = float4(inWorldPosition, 0.0, 0.0);
	mConstantBuffer.brushData = float4(mRadius, mStrength, mFalloffFraction, 0.0);
	mConstantBuffer.color = mColor;
	mConstantBuffer.worldTileRect = float4(inWorldTileRect.topLeft, inWorldTileRect.bottomRight);

	pConstantBuffer cbuf = theResourceFactory.MakeConstantBuffer(sizeof(mConstantBuffer));
	theRenderContext.UpdateSubResource(*cbuf, &mConstantBuffer);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetConstantBuffer(cbuf, 0);
	theRenderContext.CSSetTexture(rt->GetTexture(), 0);

	if (mSamplesNeighbors)
	{
		assert(inNeighborhood.size() == 4);
		if (inNeighborhood[0] != nullptr) theRenderContext.CSSetTexture(inNeighborhood[1], 1);
		if (inNeighborhood[1] != nullptr) theRenderContext.CSSetTexture(inNeighborhood[2], 2);
		if (inNeighborhood[2] != nullptr) theRenderContext.CSSetTexture(inNeighborhood[3], 3);
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

	theResourceFactory.DestroyItem(rt);
}