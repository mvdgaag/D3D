#include "PaintTool.h"
#include "BrushLibrary.h"


void PaintTool::Init(pBrushLibrary inBrushLibrary)
{
	mBrushLibrary = inBrushLibrary;
}


void PaintTool::CleanUp()
{
	mBrushLibrary = nullptr;
}


void PaintTool::OnKeyDown(unsigned int inKey)
{
	switch (inKey)
	{
	case (0x42) : // b
		mCurrentBrush = mBrushLibrary->GetBrush("Basic");
		break;
	case (0x4E) : // n
		mCurrentBrush = mBrushLibrary->GetBrush("Noise");
		break;
	case (0x53) : // s
		mCurrentBrush = mBrushLibrary->GetBrush("Smooth");
		break;
	default:
		break;
	}
}


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
	mPainting = false;
}


void PaintTool::ContinuePaint(float3 inWorldPos)
{
	ApplyPaint(float2(inWorldPos.x, inWorldPos.z));
}


void PaintTool::ApplyPaint(float2 inWorldCoord)
{
	assert(mTargetHeightField != nullptr);

	if (mCurrentBrush == nullptr)
		return;

	float2 tile_coord = mTargetHeightField->WorldToTileSpace(inWorldCoord);

	std::vector<int2> tile_indices = mTargetHeightField->GetTiles(rect(inWorldCoord - mCurrentBrush->GetRadius(), inWorldCoord + mCurrentBrush->GetRadius()));
	for each (int2 index in tile_indices)
	{
		pHeightFieldTile tile = mTargetHeightField->GetTile(index);
		int2 pixel = (tile_coord - float2(index)) * float2(tile->GetHeightTexture()->GetDimensions());
		int2 pixel_radius = int2(mCurrentBrush->GetRadius() * tile->GetPixelsPerMeter()) + 1;
		rect paint_rect(pixel - pixel_radius, pixel + pixel_radius);

		if (tile != nullptr)
			mCurrentBrush->Apply(tile, paint_rect, tile_coord * float2(tile->GetHeightTexture()->GetDimensions()));
	}
	/* TODO: required?
	for each (int2 index in tile_indices)
	{
		pHeightFieldTile tile = mTargetHeightField->GetTile(index);
		pHeightFieldTile east = mTargetHeightField->GetTile(index + int2(1, 0));
		pHeightFieldTile north = mTargetHeightField->GetTile(index + int2(0, 1));
		if (east) 
			TextureUtil::TextureStitchEast(tile->GetHeightTexture(), east->GetHeightTexture());
		if (north) 
			TextureUtil::TextureStitchNorth(tile->GetHeightTexture(), north->GetHeightTexture());
	}*/
}
