#pragma once
#include "GaagCommon.h"
#include "Input.h"

REGISTERCLASS(InputListener);

class InputListener
{
public:
	explicit InputListener() { theInput.RegisterListener(this); }
	virtual ~InputListener() { theInput.UnRegisterListener(this); } 

	virtual void OnKeyDown(unsigned int inKey)								{ UNREFERENCED_PARAMETER(inKey); }
	virtual void OnKeyUp(unsigned int inKey)								{ UNREFERENCED_PARAMETER(inKey); }
	virtual void OnMouseMove(float2 inCurrentCoord, float2 inPrevCoord)		{ UNREFERENCED_PARAMETER(inCurrentCoord); UNREFERENCED_PARAMETER(inPrevCoord); }
	virtual void OnMouseDown(int inButton)									{ UNREFERENCED_PARAMETER(inButton); }
	virtual void OnMouseUp(int inButton)									{ UNREFERENCED_PARAMETER(inButton); }
};

