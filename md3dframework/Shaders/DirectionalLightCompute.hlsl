#include "LibHelper.hlsli"
#include "LibLight.hlsli"

#define TILE_SIZE				8
#define GROUP_SIZE				(TILE_SIZE * TILE_SIZE)
#define MAX_DIRECTIONAL_LIGHTS	4
#define SHADOW_BIAS				0.0025

RWTexture2D<float4> OutDiffuseLight		: register(u0);
RWTexture2D<float4> OutSpecularLight	: register(u1);
Texture2D<float>	LinearDepth			: register(t0);
Texture2D<half2>	Normal				: register(t1);
Texture2D<float4>	Diffuse				: register(t2);
Texture2D<float4>	Surface				: register(t3);
Texture2D<float>	Shadow				: register(t4);

groupshared uint sMaxTileDepth;
groupshared uint sMinTileDepth;

cbuffer cLightingShaderConstants : register(b0)
{
	float4x4	cProjectionMatrix;
	float2		cViewReconstructionVector;
	float2		cTargetSize;
	float4		cFrameData;
};

cbuffer cDirectionalLightConstants : register(b1)
{
	float4	cLightDirection; 
	float4	cLightColor;		// rgb, alpha = has shadow map
	float4x4 cViewToLightMatrix;
	float4	cLightData;
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
	int2	coord = inDispatchThreadID.xy;
	float2 uv = (float2(coord)+0.5) / cTargetSize.xy;
	
	float	linear_depth = LinearDepth[coord];
	float3	position = ReconstructCSPosition(uv, linear_depth, cViewReconstructionVector);

	// each thread now accumulates the light for one texel
	float3 diffuse_accum = 0;
	float3 specular_accum = 0;
		
	// accumulate light from all directional light sources
	Light light;
	light.mDirection = cLightDirection.xyz;
	light.mColor = cLightColor.xyz;
	light.mHasShadowMap = cLightColor.w != 0;
	light.mAttenuation = 1.0;

	float shade = 0.0;

	if (light.mHasShadowMap)
	{
		float3 texel_light_position;
		float2 shadow_map_uv;
		float4 shadow_depth;

		texel_light_position = mul(float4(position, 1.0), cViewToLightMatrix);
		shadow_map_uv = texel_light_position.xy * float2(0.5, -0.5) + 0.5;
		shadow_depth = Shadow.Gather(PointSampler, shadow_map_uv);

		shade += (texel_light_position.z - SHADOW_BIAS < shadow_depth.x) ? 0.25 : 0.0;
		shade += (texel_light_position.z - SHADOW_BIAS < shadow_depth.y) ? 0.25 : 0.0;
		shade += (texel_light_position.z - SHADOW_BIAS < shadow_depth.z) ? 0.25 : 0.0;
		shade += (texel_light_position.z - SHADOW_BIAS < shadow_depth.w) ? 0.25 : 0.0;
	}
	if (shade > 0.0)
	{
		half3	normal = DecodeNormal(Normal[coord].xy);
		half3	diffuse = Diffuse[coord].xyz;
		half4	surface = Surface[coord];

		// setup material
		Material material;
		material.mRoughness = surface.x;
		material.mReflectivity = surface.y;
		material.mDiffuse = diffuse;

		light.mColor *= shade;
		AccumulateLight(material, position, normal, light, diffuse_accum, specular_accum);
		
		OutDiffuseLight[coord] = float4(diffuse_accum, 1);
		OutSpecularLight[coord] = float4(specular_accum, 1);
	}
	else
	{
		OutDiffuseLight[coord] = 0;
		OutSpecularLight[coord] = 0;
	}
}