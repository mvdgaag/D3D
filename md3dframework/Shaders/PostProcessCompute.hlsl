#include "LibHelper.hlsli"


RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source : register(t0);
Texture2D<float2> motionVectors : register(t1);
SamplerState sourceSampler : register(s0);
SamplerState motionSampler : register(s1);


#define MOTION_BLUR_STRENGTH (1.0)
#define MOTION_BLUR_STEPS (12)


cbuffer cPostProcessConstants : register(b0)
{
	float2	cTargetSize;
	float	cFrameID;
	float	cUnused;
};


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	float2 uv = (coord + 0.5) / cTargetSize;
	float2 mv = motionVectors.SampleLevel(motionSampler, uv, 0) * float2(1, -1);
	float2 history_uv = saturate(uv + mv * MOTION_BLUR_STRENGTH);

	float4 result = 0;
	float normalization = 0;
	for (int i = 0; i < MOTION_BLUR_STEPS; i++)
	{
		float2 samp_uv = lerp(uv, history_uv, float(i) / MOTION_BLUR_STEPS);
		float weight = float(MOTION_BLUR_STEPS - i) / (MOTION_BLUR_STEPS + 1);
		weight *= weight * weight * weight;
		result += weight * source.SampleLevel(sourceSampler, samp_uv, 0);
		normalization += weight;
	}

	float noise = Random(coord + cFrameID) * 0.01;
	float vignette = sqrt(sqrt(2) - length(uv * 2.0 - 1.0)) / sqrt(2);
	vignette = (vignette + 1.0) * 0.5;

	dst[coord] = float4(LinearToSrgb(result.xyz / normalization) * vignette + noise, 1.0);
}
