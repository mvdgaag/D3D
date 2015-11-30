#pragma once
#include "Gaag.h"
#include "HeightField.h"
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

	void SetBrush(pBrush inBrush) { mCurrentBrush = inBrush; }
	pBrush GetBrush() { return mCurrentBrush; }

	void SetTargetHeightField(pHeightField inHeightField) { mTargetHeightField = inHeightField; }
	pHeightField GetTargetHeightField() { return mTargetHeightField; }

	void OnKeyDown(unsigned int inKey) override;
	void OnKeyUp(unsigned int inKey) override  {}
	void OnMouseDown(int inButton) override;
	void OnMouseUp(int inButton) override;
	void OnMouseMove(float2 inCurrentCoord, float2 inPrevCoord) override;

private:
	void BeginPaint(float3 inWorldPos);
	void EndPaint(float3 inWorldPos);
	void ContinuePaint(float3 inWorldPos);
	
	void ApplyPaint(float2 inWorldCoord);

	pBrushLibrary mBrushLibrary;
	pBrush mCurrentBrush;
	pHeightField mTargetHeightField;
	bool mPainting;
};

