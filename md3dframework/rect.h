#pragma once
#include <math.h>
#include "int2.h"

// predeclarations
class rect;


class rect
{
public:
	int2 topLeft;
	int2 bottomRight;

	inline rect() {}

	inline rect::rect(const rect& other)
	{
		topLeft = other.topLeft;
		bottomRight = other.bottomRight;
	}

	inline rect::rect(int inBottom, int inLeft, int inTop, int inRight)
	{
		topLeft = int2(inBottom, inLeft);
		bottomRight = int2(inTop, inRight);
	}

	inline rect::rect(int2 inBottomLeft, int2 inTopRight)
	{
		topLeft = inBottomLeft;
		bottomRight = inTopRight;
	}

	inline rect::rect(int2 scalar)
	{
		topLeft = scalar;
		bottomRight = scalar;
	}

	inline rect& rect::operator = (const rect& other)
	{
		topLeft = other.topLeft;
		bottomRight = other.bottomRight;
		return *this;
	}

	inline bool rect::operator ==(const rect& other) const
	{
		return ((topLeft == other.topLeft) && (bottomRight == other.bottomRight));
	}

	inline bool rect::operator !=(const rect& other) const
	{
		return ((topLeft != other.topLeft) || (bottomRight != other.bottomRight));
	}

	inline int rect::GetWidth() const
	{
		return bottomRight.x - topLeft.x;
	}

	inline int rect::GetHeight() const
	{
		return bottomRight.y - topLeft.y;
	}

	inline int2 rect::GetCenter() const
	{
		return topLeft + int2(GetWidth() / 2, GetHeight() / 2);
	}

	inline void rect::Extend(const int2& inPoint) const
	{
		//topLeft = 
		minPerElem(topLeft, inPoint);
		//bottomRight = 
		maxPerElem(bottomRight, inPoint);
	}

	inline void rect::Extend(const rect& inRect) const
	{
		Extend(inRect.topLeft);
		Extend(inRect.bottomRight);
	}
};
