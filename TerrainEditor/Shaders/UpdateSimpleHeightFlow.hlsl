#include "LibFluid.hlsl"

RWTexture2D<float4> tValue : register(u0);
Texture2D<float4>	tHistory : register(t0);


cbuffer cConstantData : register(b0)
{
	float4 cParams;		// time_scale, friction, talus angle
	float4 cResolution;	// width, height, 1/width, 1/height
	float4 cScale;		// pixels/m x, pixels/m y, heightscale
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 coord = DTid.xy;

	// skip border pixels
	if ((coord.x < 1) || (coord.x >= cResolution.x - 1) || (coord.y < 1) || (coord.y >= cResolution.y - 1))
		return;

	// get constants
	const float time_scale = cParams.x;
	const float friction = cParams.y;
	const float talus_angle = cParams.z;

	float4 value = tHistory[coord];
	float n = tHistory[coord + int2(0, 1)].x;
	float e = tHistory[coord + int2(1, 0)].x;
	float s = tHistory[coord - int2(0, 1)].x;
	float w = tHistory[coord - int2(1, 0)].x;

	tValue[coord] = UpdateSimpleFlow(value, float4(n, e, s, w), time_scale, friction, talus_angle);
}
