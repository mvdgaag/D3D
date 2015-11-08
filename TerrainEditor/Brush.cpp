#include "Brush.h"


void Brush::Apply(pTerrainTile inTile, const rect& inPixelRect)
{
	int2 resolution = inTile->GetTexture()->GetResolution();
	rect clamped_rect = inPixelRect;
	clamped_rect.Overlap(resolution);

	if (clamped_rect.GetArea() <= 0)
		return;

	// assume all tiles are identital resolution
	mConstantBuffer.rect = float4(clamped_rect.topLeft, clamped_rect.bottomRight);
	mConstantBuffer.coord = inPixelRect.GetCenter();
	mConstantBuffer.radius = (inPixelRect.bottomRight - inPixelRect.topLeft) / 2;

	pConstantBuffer cbuf = MAKE_NEW(ConstantBuffer);
	cbuf->Init(sizeof(mConstantBuffer));
	theRenderContext.UpdateSubResource(*cbuf, &mConstantBuffer);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetConstantBuffer(cbuf, 0);
	if (inTile != nullptr) theRenderContext.CSSetRWTexture(inTile->GetRenderTarget(), 0);

	int2 num_threads = (int2(mConstantBuffer.radius) + 7) / 8;
	theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetShader(NULL);
}