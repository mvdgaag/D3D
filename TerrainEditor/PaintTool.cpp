#include "PaintTool.h"


void PaintTool::OnMouseDown(int inButton) 
{ 
	if (inButton == 0) 
		BeginPaint(Gaag.ScreenToWorldPos(theInput.GetMouseCoord())); 
}


void PaintTool::OnMouseUp(int inButton) 
{ 
	if (inButton == 0) 
		EndPaint(Gaag.ScreenToWorldPos(theInput.GetMouseCoord()));
}


void PaintTool::OnMouseMove(float2 inCurrentCoord, float2 inPrevCoord)
{ 
	if (mPainting) 
		ContinuePaint(Gaag.ScreenToWorldPos(theInput.GetMouseCoord()));
}


void PaintTool::BeginPaint(float3 inWorldPos)
{
	mPainting = true;
	ApplyPaint(float2(inWorldPos.x, inWorldPos.z));
}


void PaintTool::EndPaint(float3 inWorldPos)
{
	ApplyPaint(float2(inWorldPos.x, inWorldPos.z));
}


void PaintTool::ContinuePaint(float3 inWorldPos)
{
	ApplyPaint(float2(inWorldPos.x, inWorldPos.z));
	mPainting = false;
}


void PaintTool::ApplyPaint(float2 inWorldCoord)
{
	fprintf(stdout, "paint at coord: %f, %f, %f", inWorldCoord.x, inWorldCoord.y);
	return;


	assert(mTargetTerrain != nullptr);

	if (mCurrentBrush == nullptr)
		return;

	int2 terrain_coord = inWorldCoord + mTargetTerrain->GetWorldCenter();
	int2 tile_size = int2(mTargetTerrain->GetTileScale());
	int2 tile_coord = terrain_coord % tile_size;
	int2 tile_index = terrain_coord / tile_size;

	// take tiles around nearest crossing
	// keep tile_coord relative to bottom left corner of 2x2 tile grid
	if (tile_coord.x < tile_size.x / 2)
	{
		tile_index.x -= 1;
		tile_coord.x += tile_size.x;
	}
	if (tile_coord.y < tile_size.y / 2)
	{
		tile_index.y -= 1;
		tile_coord.y += tile_size.y;
	}

	if ((tile_index.x < 0 || tile_index.x >= mTargetTerrain->GetNumTiles().x - 1) ||
		(tile_index.y < 0 || tile_index.y >= mTargetTerrain->GetNumTiles().y - 1))
		return;

	pTerrainTile NW = mTargetTerrain->GetTile(tile_index + int2(1, 0));
	pTerrainTile NE = mTargetTerrain->GetTile(tile_index + int2(1, 1));
	pTerrainTile SW = mTargetTerrain->GetTile(tile_index + int2(0, 0));
	pTerrainTile SE = mTargetTerrain->GetTile(tile_index + int2(0, 1));
	float2 pixel_coord = float2(tile_coord) * NW->GetPixelsPerMeter();

	mCurrentBrush->Apply(NW, NE, SW, SE, pixel_coord);
}
