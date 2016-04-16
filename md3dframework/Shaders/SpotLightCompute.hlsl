#include "LibHelper.hlsli"
#include "LibLight.hlsli"

#define TILE_SIZE				8
#define GROUP_SIZE				(TILE_SIZE * TILE_SIZE)
#define MAX_SPOT_LIGHTS			128

RWTexture2D<float4> OutDiffuseLight		: register(u0);
RWTexture2D<float4> OutSpecularLight	: register(u1);
Texture2D<float>	LinearDepth			: register(t0);
Texture2D<half2>	Normal				: register(t1);
Texture2D<float4>	Diffuse				: register(t2);
Texture2D<float4>	Surface				: register(t3);
Texture2D<float4>	DiffuseLight		: register(t4);
Texture2D<float4>	SpecularLight		: register(t5);

groupshared uint sTileSpotLightIndices[MAX_SPOT_LIGHTS];
groupshared uint sTileNumSpotLights;

groupshared uint sMaxTileDepth;
groupshared uint sMinTileDepth;

cbuffer cLightingShaderConstants : register(b0)
{
	float4x4	cProjectionMatrix;
	float2		cViewReconstructionVector;
	float2		cTargetSize;
	float4		cFrameData;
};

cbuffer cSpotLightConstants : register(b1)
{
	float4	cSpotLightPositions[MAX_SPOT_LIGHTS];
	float4	cSpotLightDirections[MAX_SPOT_LIGHTS];
	float4	cSpotLightColors[MAX_SPOT_LIGHTS];
	float4	cSpotLightData; // x = count
};

SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerState PointSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};


[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void CS(uint3 inGroupID : SV_GroupID, uint3 inDispatchThreadID : SV_DispatchThreadID, uint inGroupIndex : SV_GroupIndex)
{
	// first thread initializes shared memory
	if (inGroupIndex == 0)
	{
		sTileNumSpotLights = 0;

		// TODO: read these values from depth pyramid
		sMaxTileDepth = 0;
		sMinTileDepth = 999999;
	}

	GroupMemoryBarrierWithGroupSync();

	// find min and max depth for the tile
	int2	coord =			inDispatchThreadID.xy;
	float	linear_depth =	LinearDepth[coord];
	InterlockedMin(sMinTileDepth, uint(linear_depth));
	InterlockedMax(sMaxTileDepth, uint(linear_depth + 1));
	
	GroupMemoryBarrierWithGroupSync();

	float min_tile_depth = sMinTileDepth;
	float max_tile_depth = sMaxTileDepth;

	// generate the frustum planes to test against
	float2 tile_scale = cTargetSize.xy / float(2 * TILE_SIZE);	// tiles per half screen
	float2 tile_bias = tile_scale - float2(inGroupID.xy);		// the tile offset in tile sizes, relative to middle of screen
	
	float4 horizontal =	float4(-cProjectionMatrix._11 * tile_scale.x, 0.0f, tile_bias.x, 0.0f);
	float4 vertical =	float4(0.0f, cProjectionMatrix._22 * tile_scale.y, tile_bias.y, 0.0f);
	float4 forward =	float4(0.0f, 0.0f, 1.0f, 0.0f);

	float4 frustum[6];
	frustum[0] = normalize(forward + horizontal);				// left
	frustum[1] = normalize(forward - horizontal);				// right
	frustum[2] = normalize(forward + vertical);					// bottom
	frustum[3] = normalize(forward - vertical);					// top
	frustum[4] = float4(0.0f, 0.0f, 1.0f, min_tile_depth);		// near
	frustum[5] = float4(0.0f, 0.0f, -1.0f, -max_tile_depth);	// far

	// test spot lights against the frustrum
	int num_Spot_lights = cSpotLightData.x;
	for (uint light_index = inGroupIndex; light_index < num_Spot_lights; light_index += GROUP_SIZE)
	{
		float4 light_pos = cSpotLightPositions[light_index];

		// test against all frustum planes
		bool in_frustum = true;
		for (uint i = 0; i < 6; ++i)
		{
			float distance = dot(frustum[i], float4(light_pos.xyz, 1.0f));
			in_frustum = in_frustum && (distance < light_pos.w);
		}

		// add to list if in frustum
		if (in_frustum)
		{
			uint list_index;
			InterlockedAdd(sTileNumSpotLights, 1, list_index);
			sTileSpotLightIndices[list_index] = light_index;
		}
	}

	GroupMemoryBarrierWithGroupSync();

	// fetch g-buffer data
	half3	normal = DecodeNormal(Normal[coord].xy);
	half3	diffuse = Diffuse[coord].xyz;
	half4	surface = Surface[coord];

	// each thread now accumulates the light for one texel
	float3 diffuse_accum = 0;
	float3 specular_accum = 0;

	// only light pixels in the render target	
	if (all(coord < cTargetSize.xy))
	{
		// reconstruct camera space texel_position of the texel
		float2 uv = (float2(coord) + 0.5) / cTargetSize.xy;
		float3 texel_position = ReconstructCSPosition(uv, linear_depth, cViewReconstructionVector);
		
		// accumulate light from all spot light sources
		for (int list_index = 0; list_index < sTileNumSpotLights; list_index++)
		{
			int light_index = sTileSpotLightIndices[list_index];

			float3	light_position = cSpotLightPositions[light_index].xyz;
			float	light_range = cSpotLightPositions[light_index].w;
			float3	light_vec = light_position - texel_position;
			float	range_sqr = light_range * light_range;
			float	sqr_dist = dot(light_vec, light_vec);
			float	sqrt_attenuation = max(0, 1 - sqr_dist / range_sqr);

			light_vec = normalize(light_vec);

			// attenuate by cone
			sqrt_attenuation *= (dot(cSpotLightDirections[light_index].xyz, light_vec) > cSpotLightDirections[light_index].w) ? 1.0 : 0.0;

			if (sqrt_attenuation <= 0)
				continue;

			Light light;
			light.direction = light_vec;
			light.color = cSpotLightColors[light_index].xyz;
			light.attenuation = sqrt_attenuation * sqrt_attenuation;

			// setup material
			half3	normal = DecodeNormal(Normal[coord].xy);
			half3	diffuse = Diffuse[coord].xyz;
			half4	surface = Surface[coord];
			Material material;
			material.roughness = surface.x;
			material.reflectance = surface.y;
			material.diffuse = diffuse;

			AccumulateLight(material, texel_position, normal, light, diffuse_accum, specular_accum);
		}
		
		OutDiffuseLight[coord] = DiffuseLight[coord] + float4(diffuse_accum, 1);
		OutSpecularLight[coord] = SpecularLight[coord] + float4(specular_accum, 1);
	}
}
