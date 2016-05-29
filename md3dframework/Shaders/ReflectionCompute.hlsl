#include "LibHelper.hlsli"
#include "LibLight.hlsli"


RWTexture2D<float4> Dest : register(u0);
Texture2D<float4> Source : register(t0);

Texture2D<float4> NormalTexture : register(t1);
Texture2D<float4> MaterialTexture : register(t2);
Texture2D<float4> LinearDepthTexture : register(t3);
TextureCube<float4> CubemapTexture : register(t4);
Texture2D<float4> BRDFLookupTexture : register(t5);

SamplerState NormalSampler : register(s1);
SamplerState MaterialSampler : register(s2);
SamplerState LinearDepthSampler : register(s3);
SamplerState CubemapSampler : register(s4);
SamplerState BRDFLookupSampler : register(s5);


cbuffer cConstants : register(b0)
{
	float2	cViewReconstructionVector;
	float2	cTargetSize;
	float4	cParams; // frame id, cube mips
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
	
	float m = material.x;
	float s = material.y;

	float3 view = normalize(-pos);
	float3 light = reflect(view, normal);

	float3 refl = CubemapTexture.SampleLevel(CubemapSampler, light * float3(1,-1,1), 0);
	
	refl = ApproximateSpecularIBL(CubemapTexture, CubemapSampler, cParams.y, BRDFLookupTexture, BRDFLookupSampler, float3(s, s, s), m, normal, view);
	//refl = BruteForceSpecularIBL(CubemapTexture, CubemapSampler, float3(s, s, s), m, normal, view);

	Dest[coord] = float4(refl, 0.0);
}
