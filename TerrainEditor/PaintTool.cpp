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
	assert(mTargetTerrain != nullptr);

	if (mCurrentBrush == nullptr)
		return;

	float2 tile_coord = mTargetTerrain->WorldToTileSpace(inWorldCoord);

	std::vector<int2> tile_indices = mTargetTerrain->GetTiles(rect(inWorldCoord - mCurrentBrush->GetRadius(), inWorldCoord + mCurrentBrush->GetRadius()));
	for each (int2 index in tile_indices)
	{
		pRenderTarget target = mTargetTerrain->GetLayerRenderTarget(index, mTargetLayer);
		
		// set neighbords array if needed
		apTexture neighbors;
		if (mCurrentBrush->SamplesNeighbors())
		{
			neighbors.resize(3);
			assert(tile_indices.size() <= 4);
			for each (int2 neighbor_index in tile_indices)
			{
				if (neighbor_index == index)
					continue;

				// horizontal neighbor
				if (neighbor_index.y == index.y)
					neighbors[0] = mTargetTerrain->GetLayerTexture(neighbor_index, mTargetLayer);
				// vertical neighbor
				else if (neighbor_index.x == index.x)
					neighbors[1] = mTargetTerrain->GetLayerTexture(neighbor_index, mTargetLayer);
				// diagonal neighbor
				else
					neighbors[2] = mTargetTerrain->GetLayerTexture(neighbor_index, mTargetLayer);
			}
		}

		int2 pixel = (tile_coord - float2(index)) * float2(target->GetDimensions());
		float2 pixels_per_meter = float2(target->GetDimensions().x, target->GetDimensions().y) / float2(mTargetTerrain->GetTileScale().x, mTargetTerrain->GetTileScale().y);
		int2 pixel_radius = int2(pixels_per_meter * mCurrentBrush->GetRadius()) + int2(1, 1);
		rect paint_rect(pixel - pixel_radius, pixel + pixel_radius);

		if (target != nullptr)
		{
			mCurrentBrush->Apply(target, paint_rect, tile_coord * float2(target->GetDimensions()), neighbors);
		}
	}
	/* TODO: required? should be identical, but data is duplicated and hopefully treated exactly the same
	cannot hurt to ensure with a copy

	for each (int2 index in tile_indices)
	{
		pTerrainTile tile = mTargetTerrain->GetTile(index);
		pTerrainTile east = mTargetTerrain->GetTile(index + int2(1, 0));
		pTerrainTile north = mTargetTerrain->GetTile(index + int2(0, 1));
		if (east) 
			TextureUtil::TextureStitchEast(tile->GetHeightTexture(), east->GetHeightTexture());
		if (north) 
			TextureUtil::TextureStitchNorth(tile->GetHeightTexture(), north->GetHeightTexture());
	}*/
}
