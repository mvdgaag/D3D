#pragma once
#include <math.h>
#include "GaagMath.h"

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

	inline rect::rect(int inTop, int inLeft, int inBottom, int inRight)
	{
		topLeft = int2(inTop, inLeft);
		bottomRight = int2(inBottom, inRight);
	}

	inline rect::rect(int2 inTopLeft, int2 inBottomRight)
	{
		topLeft = inTopLeft;
		bottomRight = inBottomRight;
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

	inline void rect::Extend(const int2& inPoint)
	{
		topLeft = minPerElem(topLeft, inPoint);
		bottomRight = maxPerElem(bottomRight, inPoint);
	}

	inline void rect::Extend(const rect& inRect)
	{
		Extend(inRect.topLeft);
		Extend(inRect.bottomRight);
	}

	inline void rect::Overlap(const rect& inRect)
	{
		topLeft = minPerElem(topLeft, inRect.topLeft);
		bottomRight = maxPerElem(bottomRight, inRect.bottomRight);
		// avoid negative size
		bottomRight = maxPerElem(topLeft, bottomRight);
	}

	inline int rect::GetArea()
	{
		int2 sides = bottomRight - topLeft;
		return sides.x * sides.y;
	}
};
