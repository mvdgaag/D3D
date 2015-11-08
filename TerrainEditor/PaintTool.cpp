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
	//fprintf(stdout, "paint at coord: %f, %f, %f", inWorldCoord.x, inWorldCoord.y);
	//return;

	assert(mTargetTerrain != nullptr);

	if (mCurrentBrush == nullptr)
		return;

	// todo: find overlapping tiles

	int2 tile_index;
	int2 pixel;
	if (mTargetTerrain->ToTileSpace(inWorldCoord, tile_index, pixel))
	{
		pTerrainTile tile = mTargetTerrain->GetTile(tile_index);

		int2 radius = int2(mCurrentBrush->GetRadius() * tile->GetPixelsPerMeter() + 0.5f);
		rect paint_rect(pixel - radius, pixel + radius);

		if (tile != nullptr)
			mCurrentBrush->Apply(tile, paint_rect);
	}
}
