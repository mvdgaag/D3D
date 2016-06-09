#include "LibLight.hlsli"

TextureCube<float4> CubemapTexture			: register(t0);
SamplerState CubemapSampler					: register(s0);
RWTexture2DArray<float4> OutputCubeMapFace	: register(u0);

#define TILE_SIZE	(8)
#define GROUP_SIZE	(TILE_SIZE * TILE_SIZE)

cbuffer cConstants : register(b0)
{
	float4	cParams;	// target size x, y, roughness, 0
};


// pixel to vector
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb204881(v=vs.85).aspx?tduid=(231381cd509769dc0b4cd5fd3c9a139f)(256380)(2459594)(TnL5HPStwNw-SrDvoc4s.7hEANqJyARmmw)()
float3 PixelToDirection(int3 inPixelCoord)
{
	float2 uv = ((float2)inPixelCoord.xy + float2(0.5, 0.5)) / cParams.xy;
	float2 nc = uv * 2.0 - 1.0;

	if (inPixelCoord.z == 0) // pos x
		return normalize(float3(1, -nc.y, -nc.x));

	if (inPixelCoord.z == 1) // neg x
		return normalize(float3(-1, -nc.y, nc.x));

	if (inPixelCoord.z == 2) // pos y
		return normalize(float3(nc.x,  1, nc.y));

	if (inPixelCoord.z == 3) // neg y
		return normalize(float3(nc.x, -1, -nc.y));

	if (inPixelCoord.z == 4) // pos z
		return normalize(float3(nc.x, -nc.y, 1));

	// default to neg z
	return     normalize(float3(-nc.x, -nc.y, -1));
}


[numthreads(TILE_SIZE, TILE_SIZE, 6)]
void CS(uint3 inGroupID : SV_GroupID, uint3 inDispatchThreadID : SV_DispatchThreadID, uint inGroupIndex : SV_GroupIndex)
{
	int3 coord = int3(inDispatchThreadID.xyz);

	float m = cParams.z;
	float3 R = PixelToDirection(coord);
	//float3 col = CubemapTexture.SampleLevel(CubemapSampler, R, 0);
	float3 col = PrefilterEnvMap(CubemapTexture, CubemapSampler, m*m, R);

	OutputCubeMapFace[coord] = float4(col, 1.0);
}