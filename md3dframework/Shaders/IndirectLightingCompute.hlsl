#include "LibHelper.hlsli"


RWTexture2D<float4> dst : register(u0);
Texture2D<float4> lightTexture : register(t0);
Texture2D<float2> normalTexture : register(t1);
Texture2D<float2> linearDepthTexture : register(t2);
SamplerState lightSampler : register(s0);
SamplerState normalSampler : register(s1);
SamplerState linearDepthSampler : register(s2);

cbuffer cIndirectLightingConstants : register(b0)
{
	float2	cViewReconstructionVector;
	float2	cTargetSize;
	float4	cFrameData;
};


#define NUM_THREADS 64
#define SAMPLES_PER_THREAD 8
#define DISTANCE_GROWTH (1.5f)
#define START_DISTANCE (3.0)
#define GOLDEN_ANGLE (3.1415 * (3.0 - sqrt(5.0)))

groupshared float3 colors[NUM_THREADS][SAMPLES_PER_THREAD];
groupshared float3 normals[NUM_THREADS][SAMPLES_PER_THREAD];
groupshared float3 positions[NUM_THREADS][SAMPLES_PER_THREAD];


void AccumulateSamples(float2 inUV, float inStartAngle, uint inThreadIdx)
{
	// center values
	colors[inThreadIdx][0] = lightTexture.SampleLevel(lightSampler, inUV, 0);
	
	float2 encoded_normal = normalTexture.SampleLevel(normalSampler, inUV, 0);
	normals[inThreadIdx][0] = DecodeNormal(encoded_normal);

	float depth = linearDepthTexture.SampleLevel(linearDepthSampler, inUV, 0);
	positions[inThreadIdx][0] = ReconstructCSPosition(inUV, depth, cViewReconstructionVector);

	// neighborhood samples
	float dist = START_DISTANCE;
	float2 unit_vec = float2(cos(inStartAngle), sin(inStartAngle)) / cTargetSize;
	float2x2 rot = { cos(GOLDEN_ANGLE), -sin(GOLDEN_ANGLE), cos(GOLDEN_ANGLE), sin(GOLDEN_ANGLE) };
	for (int i = 1; i < SAMPLES_PER_THREAD; i++)
	{
		float2 uv = saturate(inUV + unit_vec * dist);

		colors[inThreadIdx][i] = lightTexture.SampleLevel(lightSampler, uv, 0);

		float2 encoded_normal = normalTexture.SampleLevel(normalSampler, uv, 0);
		normals[inThreadIdx][i] = DecodeNormal(encoded_normal);

		float depth = linearDepthTexture.SampleLevel(linearDepthSampler, uv, 0);
		positions[inThreadIdx][i] = ReconstructCSPosition(inUV, depth, cViewReconstructionVector);

		dist *= DISTANCE_GROWTH;
		unit_vec = mul(unit_vec, rot);
	}
}


float3 IntegrateLighting(uint2 inGTid)
{
	float3 center_normal = normals[inGTid.y * 8 + inGTid.x][0];
	float3 center_position = positions[inGTid.y * 8 + inGTid.x][0];
	float3 accum = 0;

	uint dx = -(inGTid.x % 2) * 2 + 1;
	uint dy = -(inGTid.y % 2) * 2 + 1;
	for (int i = 0; i < SAMPLES_PER_THREAD; i++)
	{
		int idx = inGTid.y * 8 + inGTid.x;
		float3 normal = normals[idx][i];
		float3 ray = normalize(positions[idx][i] - center_position);
		float transfer_scale = saturate(dot(center_normal, ray));// * saturate(dot(normal, -ray));
		accum += transfer_scale * colors[idx][i];

		idx = inGTid.y * 8 + inGTid.x + dx;
		normal = normals[idx][i];
		ray = normalize(positions[idx][i] - center_position);
		transfer_scale = saturate(dot(center_normal, ray));// *saturate(dot(normal, -ray));
		accum += transfer_scale * colors[idx][i];

		idx = (inGTid.y + dy) * 8 + inGTid.x;
		normal = normals[idx][i];
		ray = normalize(positions[idx][i] - center_position);
		transfer_scale = saturate(dot(center_normal, ray));// *saturate(dot(normal, -ray));
		accum += transfer_scale * colors[idx][i];

		idx = (inGTid.y + dy) * 8 + inGTid.x + dx;
		normal = normals[idx][i];
		ray = normalize(positions[idx][i] - center_position);
		transfer_scale = saturate(dot(center_normal, ray));// *saturate(dot(normal, -ray));
		accum += transfer_scale * colors[idx][i];
	}

	return accum / (4 * SAMPLES_PER_THREAD);
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	uint threadIdx = GTid.y * 8 + GTid.x;
	float2 uv = (coord + 0.5) / cTargetSize;
	
	float start_angle = 0.0;
	start_angle += (3.1415 / 2.0) * (DTid.x % 2);
	start_angle += (3.1415) * (DTid.y % 2);

	AccumulateSamples(uv, start_angle, threadIdx);
	dst[coord] = float4(positions[threadIdx][0], 0);// float4(IntegrateLighting(GTid), 0);
	
	// SKIP
	dst[coord] = lightTexture[coord];
}
