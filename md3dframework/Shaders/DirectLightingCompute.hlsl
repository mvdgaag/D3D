#include "LibHelper.hlsli"
#include "LibLight.hlsli"

#define TILE_SIZE			(8)
#define GROUP_SIZE			(TILE_SIZE * TILE_SIZE)
#define MAX_POINT_LIGHTS	256
#define MAX_SPOT_LIGHTS	256


RWTexture2D<float4> OutDiffuse : register(u0);
RWTexture2D<float4> OutSpecular : register(u1);
Texture2D<float> LinearDepth : register(t0);
Texture2D<half2> Normal : register(t1);
Texture2D<float4> Diffuse : register(t2);
Texture2D<float4> Surface : register(t3);


groupshared uint sTileLightIndices[MAX_POINT_LIGHTS];
groupshared uint sTileNumLights;
groupshared uint sMaxTileDepth;
groupshared uint sMinTileDepth;


cbuffer cLightingShaderConstants : register(b0)
{
	float4x4	cProjectionMatrix;
	float2		cViewReconstructionVector;
	float2		cTargetSize;
	float4		cFrameData;
};


cbuffer cLightConstants : register(b1)
{
	float4	cLightPositions[MAX_POINT_LIGHTS];
	float4	cLightColors[MAX_POINT_LIGHTS];
	float4	cLightData; // x = count
};


cbuffer cSpotLightConstants : register(b2)
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


void CalculateLight(Material inMaterial, float3 inPosition, float3 inNormal, PointLight inLight, inout float3 ioDiffuse, inout float3 ioSpecular)
{
	float3 light_vec = inLight.Position - inPosition;
	float range_sqr = (inLight.Range * inLight.Range);
	float sqr_dist = dot(light_vec, light_vec);
	
	float attenuation = max(0, 1 - sqr_dist / range_sqr);
	if (attenuation <= 0)
		return;

	attenuation *= attenuation;

	light_vec = normalize(light_vec);
	float3 normal = inNormal;
	float3 view_vec = normalize(-inPosition);
	float3 half_vec = normalize(view_vec + light_vec);

	float ndl = saturate(dot(normal, light_vec));
	float ndv = saturate(dot(normal, view_vec));
	float ndh = saturate(dot(normal, half_vec));

	float alpha = inMaterial.Roughness * inMaterial.Roughness;
	float alpha2 = alpha * alpha;

	float3 diff = inMaterial.Diffuse * (1.0 - inMaterial.Reflectivity) * OrenNayarDiffuse(ndl, ndv, normal, view_vec, alpha);

	float F = FresnelFactor(ndv, inMaterial.Reflectivity);
	float G = GeometryFactor(ndv, ndl, alpha2);
	float D = DistributionFactor(ndh, alpha2);

	float spec = F * G * D;

	ioSpecular += inLight.Color * attenuation * spec;
	ioDiffuse += inLight.Color * attenuation * diff;
}



[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void CS(uint3 inGroupID : SV_GroupID, uint3 inDispatchThreadID : SV_DispatchThreadID, uint inGroupIndex : SV_GroupIndex)
{
	// first thread initializes shared memory
	if (inGroupIndex == 0)
	{
		sTileNumLights = 0;

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

	// each thread tests a number of lights against the frustrum
	int num_lights = cLightData.x;
	for (uint light_index = inGroupIndex; light_index < num_lights; light_index += GROUP_SIZE)
	{
		float4 light_pos = cLightPositions[light_index];

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
			InterlockedAdd(sTileNumLights, 1, list_index);
			sTileLightIndices[list_index] = light_index;
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
		// setup material
		Material material;
		material.Roughness = surface.x;
		material.Reflectivity = surface.y;
		material.Diffuse = diffuse;

		// reconstruct camera space texel_position of the texel
		float2 uv = (float2(coord) + 0.5) / cTargetSize.xy;
		float3 texel_position = ReconstructCSPosition(uv, linear_depth, cViewReconstructionVector);
		
		// accumulate light from all sources
		for (int list_index = 0; list_index < sTileNumLights; list_index++)
		{
			int light_index = sTileLightIndices[list_index];

			// setup pointlight
			PointLight light;
			light.Position = cLightPositions[light_index].xyz;
			light.Color = cLightColors[light_index].xyz;
			light.Range = cLightPositions[light_index].w;

			CalculateLight(material, texel_position, normal, light, diffuse_accum, specular_accum);
		}
	}

	OutDiffuse[coord] = float4(diffuse_accum, 1);
	OutSpecular[coord] = float4(specular_accum, 1);
}
