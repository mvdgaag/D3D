#include "Brush.h"


void Brush::Apply(pTerrainTile inTile, const rect& inPixelRect)
{
	int2 resolution = inTile->GetTexture()->GetResolution();
	if (inPixelRect.GetArea() <= 0)
		return;

	// assume all tiles are identital resolution
	mConstantBuffer.rect = int4(inPixelRect.topLeft, inPixelRect.bottomRight);
	mConstantBuffer.paintData = float4(mStrength, mFalloffFraction, inTile->GetTexture()->GetResolution());

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