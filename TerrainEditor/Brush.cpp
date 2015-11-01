#include "Brush.h"


// assumes a coordinate in the 2x2 tile rect.
void Brush::Apply(pTerrainTile inTileNW, pTerrainTile inTileNE, pTerrainTile inTileSW, pTerrainTile inTileSE, int2 inPixelCoord)
{
	// assume all tiles are identital resolution
	int2 pixel_radius = mRadius * inTileNW->GetPixelsPerMeter();

	float4 cbufdata[2] = {	float4(	inPixelCoord.x - pixel_radius.x, inPixelCoord.y - pixel_radius.y, 
									inPixelCoord.x + pixel_radius.x, inPixelCoord.y + pixel_radius.y),
							float4(	inPixelCoord.x, inPixelCoord.y, pixel_radius.x, pixel_radius.y) };

	assert(cbufdata[0].x > 0 && cbufdata[0].y > 0 && 
		cbufdata[0].z < 2 * inTileNW->GetTexture()->GetWidth() &&
		cbufdata[0].w < 2 * inTileNW->GetTexture()->GetHeight() );

	pConstantBuffer cbuf = MAKE_NEW(ConstantBuffer);
	cbuf->Init(sizeof(cbufdata));
	theRenderContext.UpdateSubResource(*cbuf, &cbufdata);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetConstantBuffer(cbuf, 0);
	theRenderContext.CSSetRWTexture(inTileNW->GetRenderTarget(), 0);
	theRenderContext.CSSetRWTexture(inTileNE->GetRenderTarget(), 1);
	theRenderContext.CSSetRWTexture(inTileSW->GetRenderTarget(), 2);
	theRenderContext.CSSetRWTexture(inTileSE->GetRenderTarget(), 3);

	int2 num_threads = (2 * pixel_radius + 7) / 8;
	theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);

	theRenderContext.CSSetRWTexture(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 1);
	theRenderContext.CSSetRWTexture(NULL, 2);
	theRenderContext.CSSetRWTexture(NULL, 3);
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetShader(NULL);
}