#include "LibLight.hlsli"

TextureCube<float4> CubemapTexture	: register(t0);
SamplerState CubemapSampler			: register(s0);
RWTexture2D<float4> OutputCubeMap	: register(u0);

#define TILE_SIZE	(8)
#define GROUP_SIZE	(TILE_SIZE * TILE_SIZE)

cbuffer cConstants : register(b0)
{
	float4	cParams;	// target size x, y, roughness, unused
};


// pixel to vector
float3 PixelToDirection(int2 inPixelCoord, int inAxis)
{
	float2 uv = ((float2)inPixelCoord + float2(0.5, 0.5)) / cParams.xy;
	float2 angle = HALF_PI * (uv - float2(0.5, 0.5));

	if (inAxis == 0) // pos x
		return normalize(float3(1.0, sin(angle.x), sin(angle.y)));
	if (inAxis == 1) // neg x
		return normalize(float3(1.0, sin(angle.x), sin(angle.y)));
	if (inAxis == 2) // pos y
		return normalize(float3(sin(angle.x),  1.0, sin(angle.y)));
	if (inAxis == 3) // neg y
		return normalize(float3(sin(angle.x), -1.0, sin(angle.y)));
	if (inAxis == 4) // pos z
		return normalize(float3(sin(angle.x), sin(angle.y),  1.0));
	if (inAxis == 5) // neg z
		return normalize(float3(sin(angle.x), sin(angle.y), -1.0));

	return float3(0.0, 0.0, 0.0);
}


[numthreads(TILE_SIZE, TILE_SIZE, 6)]
void CS(uint3 inGroupID : SV_GroupID, uint3 inDispatchThreadID : SV_DispatchThreadID, uint inGroupIndex : SV_GroupIndex)
{
	int2 coord = inDispatchThreadID.xy;
	int face = inDispatchThreadID.z;

	float m = cParams.z;
	float3 R = PixelToDirection(coord, face);
	float3 col = PrefilterEnvMap(CubemapTexture, CubemapSampler, m, R);

	OutputCubeMap[coord] = float4(col, 1.0);
}