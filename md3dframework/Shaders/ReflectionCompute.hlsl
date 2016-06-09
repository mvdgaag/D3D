#include "LibHelper.hlsli"
#include "LibLight.hlsli"


RWTexture2D<float4> Dest : register(u0);
Texture2D<float4> Source : register(t0);

Texture2D<float4> NormalTexture : register(t1);
Texture2D<float4> MaterialTexture : register(t2);
Texture2D<float4> DiffuseTexture : register(t3);
Texture2D<float4> LinearDepthTexture : register(t4);
TextureCube<float4> CubemapTexture : register(t5);
Texture2D<float4> BRDFLookupTexture : register(t6);

SamplerState NormalSampler : register(s1);
SamplerState MaterialSampler : register(s2);
SamplerState DiffuseSampler : register(s3);
SamplerState LinearDepthSampler : register(s4);
SamplerState CubemapSampler : register(s5);
SamplerState BRDFLookupSampler : register(s6);


cbuffer cConstants : register(b0)
{
	float2	cViewReconstructionVector;
	float2	cTargetSize;
	float4	cParams; // frame id, cube mips
	float4x4 cInverseViewMatrix;
};


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	float2 uv = (coord + 0.5) / cTargetSize;

	float depth = LinearDepthTexture.SampleLevel(LinearDepthSampler, uv, 0).x;
	float3 pos = ReconstructCSPosition(uv, depth, cViewReconstructionVector);
	float3 normal = DecodeNormal(NormalTexture.SampleLevel(NormalSampler, uv, 0).xy);
	float4 material = MaterialTexture.SampleLevel(MaterialSampler, uv, 0);
	
	float roughness = max(0.001, material.x);
	float3 specular_color = material.yyy;
	float3 diffuse_color = DiffuseTexture[coord];
	float metal = material.z;

	// DEVHACK, set metal for high reflectance values (over 20%)
	// metal = saturate(material.y * 5.0 - 1.0);

	specular_color = lerp(specular_color, (specular_color * 2 + 0.5) * diffuse_color, metal.xxx);
	diffuse_color *= (1 - metal);

	float3 world_view = mul((float3x3)cInverseViewMatrix, normalize(-pos)).xyz;
	float3 world_normal = mul((float3x3)cInverseViewMatrix, normal).xyz;

	float3 refl = ApproximateSpecularIBL(CubemapTexture, CubemapSampler, cParams.y, BRDFLookupTexture, BRDFLookupSampler, specular_color, roughness, world_normal, world_view);
	// DEVHACK: add diffuse
	refl += diffuse_color * (1 - specular_color) * CubemapTexture.SampleLevel(CubemapSampler, world_normal, 9).rgb;

	if (coord.x > 400 && false)
	{
		refl = BruteForceSpecularIBL(CubemapTexture, CubemapSampler, specular_color, roughness, world_normal, world_view);
		// DEVHACK: add diffuse
		refl += diffuse_color * (1 - specular_color) * BruteForceDiffuseIBL(CubemapTexture, CubemapSampler, specular_color, roughness, world_normal, world_view);
	}

	Dest[coord] = float4(refl, 0.0);
}
