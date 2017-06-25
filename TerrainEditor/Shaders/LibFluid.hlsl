#ifndef LIB_FLUID
#define LIB_FLUID


float4 UpdateSimpleFlow(float4 inValues, float4 inNeighborHeights, float inTimeScale, float inFriction, float inTalusAngle)
{
	// find the data for this texel and its neighbors. The z component holds the unaffected last frame's value
	float4 c = inValues;
	float n = inNeighborHeights.x;
	float e = inNeighborHeights.y;
	float s = inNeighborHeights.z;
	float w = inNeighborHeights.w;

	float height_diff = ((n + e + s + w) / 4.0 - c.x);
	height_diff = height_diff < 0.0 ? min(0.0, height_diff + inTalusAngle) : max(0.0, height_diff - inTalusAngle);

	float velocity = inFriction * c.y + inTimeScale * height_diff;
	float height = max(0.0, c.x + velocity);

	return float4(height, velocity, 0.0, 0.0);
}

#endif