#include "LibHelper.hlsli"


RWTexture2D<float4> dst : register(u0);
Texture2D<float4> sourceTexture : register(t0);
Texture2D<float2> normalTexture : register(t1);
Texture2D<float> linearDepthTexture : register(t2);
SamplerState sourceSampler : register(s0);
SamplerState normalSampler : register(s1);
SamplerState linearDepthSampler : register(s2);

cbuffer cIndirectLightingConstants : register(b0)
{
	float2	cViewReconstructionVector;
	float2	cTargetSize;
	float4	cFrameData;
};


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	float2 uv = (coord + 0.5) / cTargetSize;

	float depth = linearDepthTexture[coord];
	float3 pos = ReconstructCSPosition(uv, depth, cViewReconstructionVector);
	float3 normal = DecodeNormal(normalTexture[coord].xy);

	float4 result = sourceTexture[coord];
	float running_weight = 1.0;

	int width = cFrameData.y;
	int dir = (cFrameData.x == 0) ? 0.0 : 1.0;
	int2 offset = int2(dir, 1.0 - dir);
	int2 samp_coord = coord - (width / 2) * offset;

	[unroll(5)]
	for (int i = 0; i < width; i++, samp_coord += offset)
	{
		if (i == width/2)
			continue;
		
		float2 samp_uv = (samp_coord + 0.5) / cTargetSize;
		float samp_depth = linearDepthTexture[samp_coord];
		float3 samp_pos = ReconstructCSPosition(samp_uv, samp_depth, cViewReconstructionVector);
		float3 samp_dir = normalize(samp_pos - pos);
		float3 samp_normal = DecodeNormal(normalTexture[samp_coord].xy);
		float weight = saturate(dot(normal, samp_normal)) * (1.0 - abs(dot(normal, samp_dir)));
		weight *= weight;	// gamma
		weight *= 2.0;		// gain
		weight -= 0.1;		// lift
		weight = saturate(weight);

		result += sourceTexture[samp_coord] * weight;
		running_weight += weight;
	}

	dst[coord] = result / running_weight;
}
