#include "LibHelper.hlsli"
#include "LibLight.hlsli"

#define TILE_SIZE				8
#define GROUP_SIZE				(TILE_SIZE * TILE_SIZE)
#define MAX_DIRECTIONAL_LIGHTS	4
#define SHADOW_BIAS				0.001
#define NUM_SHADOW_SAMPLES		32
#define LIGHT_RADIUS			10

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
	float4		cFrameData; // x = frameID, y = random
};

cbuffer cDirectionalLightConstants : register(b1)
{
	float4	cLightDirection; 
	float4	cLightColor;			// rgb, alpha = has shadow map
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
	int2 coord = inDispatchThreadID.xy;
	float2 uv = (float2(coord)+0.5) / cTargetSize.xy;
	
	float linear_depth = LinearDepth[coord];
	float3 position = ReconstructCSPosition(uv, linear_depth, cViewReconstructionVector);

	// each thread now accumulates the light for one texel
	float3 diffuse_accum = 0;
	float3 specular_accum = 0;
		
	// accumulate light from all directional light sources
	Light light;
	light.direction = cLightDirection.xyz;
	light.color = cLightColor.xyz;
	light.hasShadowMap = cLightColor.w != 0;
	light.attenuation = 1.0;

	float shade = 0.0;

	if (light.hasShadowMap)
	{
		float3 texel_light_position = mul(float4(position, 1.0), cViewToLightMatrix);
		float2 shadow_map_uv = texel_light_position.xy * float2(0.5, -0.5) + 0.5;
		int2 shadow_map_resolution;
		Shadow.GetDimensions(shadow_map_resolution.x, shadow_map_resolution.y);
		float light_radius_uv = LIGHT_RADIUS / float(shadow_map_resolution.x);

		for (int i = 0; i < NUM_SHADOW_SAMPLES; i++)
		{
			float rnd = Random(coord * i * cFrameData.x) * 2.0 * 3.1415;
			float2 unit_dir = float2(sin(rnd), cos(rnd));
			float r = sqrt(float(i) / NUM_SHADOW_SAMPLES);
			float2 ruv = shadow_map_uv + light_radius_uv * r * unit_dir;
			
			float shadow_depth = Shadow.SampleLevel(PointSampler, ruv, 0) + SHADOW_BIAS;
			float texel_depth = texel_light_position.z;

			//if ((texel_depth - shadow_depth) < light_radius_uv * r)
			if ((texel_depth - shadow_depth) / texel_depth < r * 0.0)
				shade += 1.0 / NUM_SHADOW_SAMPLES;
		}
	}
	else
	{
		shade = 1.0;
	}
	if (shade > 0.0)
	{
		half3 normal = DecodeNormal(Normal[coord].xy);
		half3 diffuse = Diffuse[coord].xyz;
		half4 surface = Surface[coord];

		Material material;
		material.roughness = surface.x;
		material.reflectance = surface.y;
		material.diffuse = diffuse;
		material.metalicity = surface.z;

		light.color *= shade;
		
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
