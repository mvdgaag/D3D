#include "LibHelper.hlsli"


RWTexture2D<float4> dst : register(u0);
Texture2D<float4> lightTexture : register(t0);
Texture2D<float2> normalTexture : register(t1);
Texture2D<float2> linearDepthTexture : register(t2);
SamplerState lightSampler : register(s0);
SamplerState normalSampler : register(s1);
SamplerState linearDepthSampler : register(s2);

/*
#define NUM_THREADS 64
#define SAMPLES_PER_THREAD 8
#define DISTANCE_GROWTH (1.5f)
#define GOLDEN_ANGLE (3.1415 * (3.0 - sqrt(5.0)))

groupshared float3 colors[NUM_THREADS][SAMPLES_PER_THREAD];
groupshared float3 normals[NUM_THREADS][SAMPLES_PER_THREAD];
groupshared float3 positions[NUM_THREADS][SAMPLES_PER_THREAD];


void AccumulateSamples(float2 inUV, float inStartAngle, uint inThreadIdx)
{
	// center values
	colors[inThreadIdx][0] = lightTexture.SampleLevel(lightSampler, inUV, 0);
	normals[inThreadIdx][0] = normalTexture.SampleLevel(normalSampler, inUV, 0);
	depths[inThreadIdx][0] = linearDepthTexture.SampleLevel(linearDepthSampler, inUV, 0);

	// neighborhood samples
	float dist = 1.0;
	float2 unit_vec = float2(cos(inStartAngle), sin(inStartAngle));
	float2x2 rot = { cos(GOLDEN_ANGLE), -sin(GOLDEN_ANGLE), cos(GOLDEN_ANGLE), sin(GOLDEN_ANGLE) };
	for (int i = 1; i < SAMPLES_PER_THREAD; i++)
	{
		float2 uv = inUv + unit_vec * dist;
		colors[inThreadIdx][i] = lightTexture.SampleLevel(lightSampler, uv, 0);
		normals[inThreadIdx][i] = DecodeNormal(normalTexture.SampleLevel(normalSampler, uv, 0).xy);
		positions[inThreadIdx][i] = ReconstructCSPosition(uv, linearDepthTexture.SampleLevel(linearDepthSampler, uv, 0));
		dist *= DISTANCE_GROWTH;
		unit_vec *= rot;
	}
}


float4 IntegrateLighting(uint inThreadIdx)
{
	// TODO: out of range, what to do with borders?
	// just shift neighborhood?
	// fewer samples?
	// smaller 2x2 neighborhood, similar to ddx ddy?
	// second pass?

	float3 center_normal = normals[inThreadIdx][0];
	float4 accum = 0;
	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			int idx = inThreadIdx + y * 3 + x;
			for (int i = 0; i < SAMPLES_PER_THREAD; i++)
			{
				float3 normal = normals[idx][i];
				float transfer_scale = saturate(dot(normal, -center_normal)); // just normals, no positions now
				accum += transfer_scale * colors[idx][i];
			}
		}
	}
	return accum / (9 * SAMPLES_PER_THREAD);
}
*/

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	dst[coord] = lightTexture[coord];

	/*
	accumulate samples (normal, direct, history indirect, depth (for pos))
	calculate visibilities
	transfer light
	*/
}
