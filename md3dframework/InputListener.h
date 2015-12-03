#pragma once
#include "GaagCommon.h"
#include "Input.h"

REGISTERCLASS(InputListener);

class InputListener
{
public:
	explicit InputListener() { theInput.RegisterListener(this); }
	virtual ~InputListener() { theInput.UnRegisterListener(this); } 

	virtual void OnKeyDown(unsigned int inKey) {}
	virtual void OnKeyUp(unsigned int inKey) {}
	virtual void OnMouseMove(float2 inCurrentCoord, float2 inPrevCoord) {}
	virtual void OnMouseDown(int inButton) {}
	virtual void OnMouseUp(int inButton) {}
};

