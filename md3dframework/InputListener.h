#pragma once
#include "LinearAlgebra.h"


class InputListener
{
public:
	InputListener() {};
	virtual ~InputListener() {};

	virtual void OnKeyDown(unsigned int inKey) {};
	virtual void OnKeyUp(unsigned int inKey) {};
	virtual void OnMouseMove(float2 inCurrentCoord, float2 inPrevCoord) {};
	virtual void OnMouseDown(int inButton) {};
	virtual void OnMouseUp(int inButton) {};
};

