#include "LibLight.hlsli"

RWTexture2D<float4> OutputTexture	: register(u0);

#define TILE_SIZE	(8)
#define GROUP_SIZE	(TILE_SIZE * TILE_SIZE)


cbuffer cConstants : register(b0)
{
	float4	cParams;	// target size x, y
};



[numthreads(TILE_SIZE, TILE_SIZE, 6)]
void CS(uint3 inGroupID : SV_GroupID, uint3 inDispatchThreadID : SV_DispatchThreadID, uint inGroupIndex : SV_GroupIndex)
{
	int2 coord = inDispatchThreadID.xy;
	float2 uv = ((float2)coord + float2(0.5, 0.5)) / cParams.xy;

	float m = uv.x;
	float ndv = uv.y;
	float2 col = IntegrateBRDF(m, ndv);

	OutputTexture[coord] = float4(col, 0.0, 1.0);
}