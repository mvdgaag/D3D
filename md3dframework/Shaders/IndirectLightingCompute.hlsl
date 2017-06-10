#include "LibHelper.hlsli"

RWTexture2D<float4> dst : register(u0);					// full or half res
Texture2D<float4> lightTexture : register(t0);			// quarter res
Texture2D<float2> normalTexture : register(t1);			// full or half res
Texture2D<float> linearDepthTexture : register(t2);		// full or half res
Texture2D<float4> diffuseTexture : register(t3);		// full res
Texture2D<float> maxDepthTexture : register(t4);		// full res, 5 mips

SamplerState lightSampler : register(s0);
SamplerState normalSampler : register(s1);
SamplerState linearDepthSampler : register(s2);
SamplerState diffuseSampler : register(s3);
SamplerState maxDepthSampler : register(s4);

cbuffer cIndirectLightingConstants : register(b0)
{
	float2	cViewReconstructionVector;
	float2	cTargetSize;
	float4	cFrameData; // world_up.xyz, random 0..1
};

#define MAX_SAMPLES 32
#define MAX_MIP_LEVEL 7
#define GOLDEN_ANGLE (3.1415 * (3.0 - sqrt(5.0)))


float4 SSGI(float3 inPos, float3 inNormal, float inDepth, float2 inCoord)
{
	float3 pos = inPos;
	float3 normal = inNormal;
	float depth = inDepth;
	float2 coord = inCoord;

	float cs_radius = min(20.0, depth);
	float sqr_radius = cs_radius * cs_radius;
	float ss_radius = cTargetSize.x * cs_radius * cViewReconstructionVector.x / depth;

	float pattern[16] = {	0.0, 8.0, 2.0, 10.0,
							12.0, 4.0, 14.0, 6.0,
							3.0, 11.0, 1.0, 9.0,
							15.0, 7.0, 13.0, 5.0 };

	float rnd = Random(coord + cFrameData.w);
	//rnd = Random(floor(100.0 * pos));
	//rnd = 0.0;
	//float rnd = cFrameData.w;
	int irnd = cFrameData.w * 16.0;

	float start_angle = (pattern[((coord.y % 4) * 4 + (coord.x % 4) + irnd) % 16]) / 16.0 * 2.0 * 3.1415;
	float2 unit_vec = float2(sin(start_angle), cos(start_angle));

	float cosa = cos(GOLDEN_ANGLE);
	float sina = sin(GOLDEN_ANGLE);
	float2x2 rot_mat = float2x2(cosa, -sina, sina, cosa);

	float radius = 0.9 + 0.2 * rnd;
	float accum_ao = 0.0;
	float3 accum_radiance = float3(0.0, 0.0, 0.0);
	float running_weight_ao = 0.0001;
	float running_weight_radiance = 1.0;

	for (uint i = 0; i < MAX_SAMPLES, radius < ss_radius; i++, radius = max(radius * 1.25, radius + 2.0))
	{
		int2 samp_coord = coord + (unit_vec * radius);
		if (any(samp_coord < int2(0, 0)) || any(samp_coord > int2(cTargetSize.xy) - 1))
			continue;
		
		float2 samp_uv = float2(samp_coord + 0.5) / cTargetSize;
		float samp_depth = maxDepthTexture.SampleLevel(lightSampler, samp_uv, clamp((i-2) / 2, 0, MAX_MIP_LEVEL)).x * 1.001;
		float3 samp_pos = ReconstructCSPosition(samp_uv, samp_depth, cViewReconstructionVector);
		float3 samp_normal = DecodeNormal(normalTexture.SampleLevel(lightSampler, samp_uv, 0).xy);

		float3 samp_vec = samp_pos - pos;
		float sqr_dist = dot(samp_vec, samp_vec);
		float vdn = max(0.0, dot(normal, samp_vec));

		float dist_weight = pow(max(0.0, (sqr_radius - sqr_dist) / sqr_radius), 3);
		accum_ao += dist_weight * vdn / sqrt(sqr_dist);
		running_weight_ao += dist_weight;

		float weight_radiance = vdn * dist_weight;
		weight_radiance *= dot(samp_normal, samp_vec) < 0 ? 1.0 : 0.0;
		accum_radiance += weight_radiance * lightTexture.SampleLevel(lightSampler, samp_uv, 0).xyz;
		running_weight_radiance += weight_radiance;

		unit_vec = mul(unit_vec, rot_mat);
	}

	accum_ao /= running_weight_ao;
	accum_ao = 1.001 - saturate(2 * accum_ao);

	accum_radiance /= running_weight_radiance;

	return float4(accum_radiance, accum_ao);
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	float2 uv = (coord + 0.5) / cTargetSize;

	float depth = linearDepthTexture.SampleLevel(lightSampler, uv, 0).x;
	float3 pos = ReconstructCSPosition(uv, depth, cViewReconstructionVector);
	float3 normal = DecodeNormal(normalTexture.SampleLevel(lightSampler, uv, 0).xy);

	float4 ssgi = SSGI(pos, normal, depth, coord);
	
	float3 up = cFrameData.xyz;
	float3 sky_color = float3(0.5, 0.5, 0.5); // TODO: add skylight light instead

	// fraction of hemisphere
	float sky_visible_frac = saturate(1.0 - acos(dot(normal, up) - 0.01) / 3.1415);
	float sky_average_attenuation = dot(up, normalize(normal + up));
	
	ssgi.rgb += sky_visible_frac * sky_average_attenuation * ssgi.a * sky_color;
	ssgi.rgb *= diffuseTexture.SampleLevel(diffuseSampler, uv, 0).xyz;

	dst[coord] = float4(ssgi);
}
