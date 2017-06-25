#include "LibFluid.hlsl"

RWTexture2D<float4> tValue		: register(u0);
Texture2D<float4>	tHistory	: register(t0);
Texture2D<float4>	tNorth		: register(t1);
Texture2D<float4>	tEast		: register(t2);
Texture2D<float4>	tSouth		: register(t3);
Texture2D<float4>	tWest		: register(t4);


cbuffer cConstantData : register(b0)
{
	float4 cParams;		// time_scale, friction, talus angle
	float4 cResolution;	// width, height, 1/width, 1/height
	float4 cScale;		// pixels/m x, pixels/m y, heightscale
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int select = DTid.y;

	int2 coord;
	int2 tex_size = cResolution.xy;

	if (select == 0)		coord = int2(DTid.x, tex_size.y - 1);
	else if (select == 1)	coord = int2(tex_size.x - 1, DTid.x);
	else if (select == 2)	coord = int2(DTid.x, 0);
	else					coord = int2(0, DTid.x);

	// get constants
	const float time_scale = cParams.x;
	const float friction = cParams.y;
	const float talus_angle = cParams.z;

	float4 value = tHistory[coord];
	float n = (coord.y == tex_size.y - 1)	? tNorth[int2(coord.x, 1)]				: tHistory[coord + int2(0, 1)];
	float e = (coord.x == tex_size.x - 1)	? tEast[int2(1, coord.y)]				: tHistory[coord + int2(1, 0)];
	float s = (coord.y == 0)				? tSouth[int2(coord.x, tex_size.y - 2)] : tHistory[coord - int2(0, 1)];
	float w = (coord.x == 0)				? tWest[int2(tex_size.x - 2, coord.y)]	: tHistory[coord - int2(1, 0)];

	tValue[coord] = UpdateSimpleFlow(value, float4(n, e, s, w), time_scale, friction, talus_angle);
}
