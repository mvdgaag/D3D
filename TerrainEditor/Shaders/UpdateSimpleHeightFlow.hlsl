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
	// skip border pixels
	int2 coord = DTid.xy;

	if ((coord.x < 0) || (coord.x >= cResolution.x) || (coord.y < 0) || (coord.y >= cResolution.y))
		return;

	// get constants
	const float time_scale = cParams.x;
	const float friction = cParams.y;
	const float talus_angle = cParams.z;

	// find the data for this texel and its neighbors. The z component holds the unaffected last frame's value
	float4 c = tHistory[coord];
	float n = tHistory[coord + int2(0, 1)].x;
	float e = tHistory[coord + int2(1, 0)].x;
	float s = tHistory[coord - int2(0, 1)].x;
	float w = tHistory[coord - int2(1, 0)].x;

	float height_diff = ((n + e + s + w) / 4.0 - c.x);
	height_diff = height_diff < 0.0 ? min(0.0, height_diff + talus_angle) : max(0.0, height_diff - talus_angle);

	float velocity = friction * c.y + time_scale * height_diff;
	float height = max(0.0, c.x + velocity);

	tValue[coord] = float4(height, velocity, 0.0, 0.0);
}
