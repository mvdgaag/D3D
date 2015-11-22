#pragma once
#include "GaagCommon.h"
#include "Input.h"

PREDEFINE(InputListener, pInputListener);

class InputListener
{
public:
	explicit InputListener() { theInput.RegisterListener(this); }
	// cannot make smart pointer to itself in destructor, because it would lead to an infinite loop
	virtual ~InputListener() { theInput.UnRegisterListener(this); } 

	virtual void OnKeyDown(unsigned int inKey) {}
	virtual void OnKeyUp(unsigned int inKey) {}
	virtual void OnMouseMove(float2 inCurrentCoord, float2 inPrevCoord) {}
	virtual void OnMouseDown(int inButton) {}
	virtual void OnMouseUp(int inButton) {}
};

