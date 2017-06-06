#pragma once
#include "Gaag.h"
#include "Terrain.h"
#include "Brush.h"

REGISTERCLASS(PaintTool);
REGISTERCLASS(BrushLibrary);

class PaintTool : public InputListener
{
public:
	PaintTool() { mPainting = false; }
	~PaintTool() {}

	void Init(pBrushLibrary inBrushLibrary);
	void CleanUp();

	void SetBrush(pBrush inBrush) { assert(inBrush != nullptr);  mCurrentBrush = inBrush; }
	pBrush GetBrush() { return mCurrentBrush; }

	void SetTargetTerrain(pTerrain inTerrain) { assert(inTerrain != nullptr); mTargetTerrain = inTerrain; }
	pTerrain GetTargetTerrain() { return mTargetTerrain; }

	void OnKeyDown(unsigned int inKey) override;
	void OnKeyUp(unsigned int inKey) override  {}
	void OnMouseDown(int inButton) override;
	void OnMouseUp(int inButton) override;
	void OnMouseMove(float2 inCurrentCoord, float2 inPrevCoord) override;

private:
	void BeginPaint(float3 inWorldPos);
	void EndPaint(float3 inWorldPos);
	void ContinuePaint(float3 inWorldPos);
	
	void ApplyPaint(float2 inWorldCoord, const int inLayerID = 0);

	pBrushLibrary mBrushLibrary;
	pBrush mCurrentBrush;
	pTerrain mTargetTerrain;
	bool mPainting;
};

