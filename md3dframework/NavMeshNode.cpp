#include "NavMeshNode.h"
#include <algorithm>


aFloat2 NavMeshNode::CalculatePath(float2 inPos, float2 inTarget)
{
	aFloat2 result;
	return result;
}


bool NavMeshNode::IsConvex()
{
	assert(mPoints.size() >= 3);
	int num_points = mPoints.size();
	float2 last_dir = mPoints.back() - mPoints.front();
	for (int i = 0; i < num_points - 1; i++)
	{
		float2 dir = mPoints[i + 1] - mPoints[i];
		float cross = dir.x * last_dir.y - dir.y * last_dir.x;
		if (cross < 0.0f)
			return false;
		last_dir = dir;
	}
	return true;
}


apNavMeshNode NavMeshNode::SplitToConvex()
{
	assert(mPoints.size() >= 3);

	apNavMeshNode result;
	result.push_back(this);

	// loop through all points
	int num_points = mPoints.size();
	float2 last_dir = mPoints.back() - mPoints.front();
	for (int i = 0; i < num_points - 1; i++)
	{
		float2 dir = mPoints[i + 1] - mPoints[i];
		float cross = dir.x * last_dir.y - dir.y * last_dir.x;
		
		// is this point to the right side: non convex point
		if (cross < 0.0f)
		{
			pNavMeshNode next_node = new NavMeshNode();
			
			// move remaining points to next node
			for (int j = i; j < num_points; j++)
				next_node->mPoints.push_back(mPoints[j]);
			mPoints.erase(mPoints.begin() + i, mPoints.end());

			// iterate this function on next node and append the result
			apNavMeshNode next_array = next_node->SplitToConvex();
			result.insert(result.end(), next_array.begin(), next_array.end());
			return result;
		}
		last_dir = dir;
	}
	
	return result;
}


float2 NavMeshNode::CalcCentroid()
{
	float2 sum(0.0f);
	for each (float2 point in mPoints)
		sum += point;
	return sum / float(mPoints.size());

}