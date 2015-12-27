#pragma once

#include "GaagCommon.h"

REGISTERCLASS(NavMeshNode);


class NavMeshNode
{
public:
	NavMeshNode()	{};
	~NavMeshNode()	{};

	aFloat2 CalculatePath(float2 inPos, float2 inTarget);

private:
	aFloat2			mPoints;
	aNavMeshNode	mNeighbors;

	bool			IsConvex();
	apNavMeshNode	SplitToConvex();
	float2			CalcCentroid();
};
