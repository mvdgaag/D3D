#include "LibNormals.hlsli"

Texture2D cDiffuseTexture : register(t0);
Texture2D cNormalTexture : register(t1);
Texture2D cMaterialTexture : register(t2);

SamplerState cDiffuseSampler : register(s0);
SamplerState cNormalSampler : register(s1);
SamplerState cMaterialSampler : register(s2);


struct PS_INPUT
{
	float4 Position			: SV_POSITION;
	float3 Normal			: NORMAL;
	float3 Tangent			: TANGENT;
	float2 TexCoord			: TEXCOORD0;
	float2 MotionVectors	: TEXCOORD1;
};


struct PS_OUTPUT
{
	float LinearDepth		: SV_Target0;
	half2 Normal			: SV_Target1;
	float4 Diffuse			: SV_Target2;
	float4 Material			: SV_Target3;
	half2 MotionVectors		: SV_Target4;
};


float3 NormalMap(float3 inGeometryNormal, float3 inTangent, float2 inTexCoord)
{
	float3 result = inGeometryNormal;
	result += 0.0001 * cNormalTexture.Sample(cNormalSampler, inTexCoord).xyz;
	return result;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;
	output.LinearDepth = input.Position.z;
	output.Diffuse = cDiffuseTexture.Sample(cDiffuseSampler, input.TexCoord);
	output.Normal = AzimuthalEncodeNormal(NormalMap(input.Normal, input.Tangent, input.TexCoord));
	output.Material = cMaterialTexture.Sample(cMaterialSampler, input.TexCoord);
	output.MotionVectors = input.MotionVectors;
	return output;
}
