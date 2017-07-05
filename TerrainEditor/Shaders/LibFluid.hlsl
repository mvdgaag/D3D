#ifndef LIB_FLUID
#define LIB_FLUID


float4 UpdateSimpleFlow(float4 inValues, float4 inNeighborHeights, float3 inScale, float inTimeScale, float inFriction, float inTalusSlope)
{
	float4 c = inValues;
	float n = inNeighborHeights.x;
	float e = inNeighborHeights.y;
	float s = inNeighborHeights.z;
	float w = inNeighborHeights.w;

	float2 slope = inScale.z * float2(	max(abs(c.x - w), abs(c.x - e)),
										max(abs(c.x - s), abs(c.x - n))) / inScale.xy;
	float talus = length(slope) > inTalusSlope;
	
	float height_diff = (n + e + s + w) * 0.25 - c.x;
	float velocity = talus * inFriction * c.y + inTimeScale * talus * height_diff;
	float height = max(0.0, c.x + velocity);

	return float4(height, velocity, 0.0, 0.0);
}

#endif