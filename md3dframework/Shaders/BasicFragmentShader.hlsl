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



float3x3 CotangentFrame(float3 inNormal, float3 inCSPos, float2 inUv)
{

}


float3 PerturbNormal(float3 inNormal, float3 inViewVec, float2 inUv)
{
	// calculate derivatives
	float3 dpx = ddx(inViewVec);
	float3 dpy = ddy(inViewVec);
	float2 dtx = ddx(inUv);
	float2 dty = ddy(inUv);

	// make new ortaghonal system
	float3 dp2perp = cross(dpy, inNormal);
	float3 dp1perp = cross(inNormal, dpx);
	float3 tangent = dp2perp * dtx.x + dp1perp * dty.x;
	float3 bitanget = dp2perp * dtx.y + dp1perp * dty.y;

	// generate TBN matrix
	float max_rcp = rsqrt(max(dot(tangent, tangent), dot(bitanget, bitanget)));
	float3x3 TBN = float3x3(tangent * max_rcp, bitanget * max_rcp, inNormal);

	// map normals
	float3 map = cNormalTexture.Sample(cNormalSampler, inUv).xyz * 2.0 - 1.0;
	map.y *= -1;
	return normalize(mul(TBN, map));
}


float3 NormalMap(float3 inGeometryNormal, float3 inTangent, float2 inTexCoord)
{
	float3 normal = normalize(inGeometryNormal);
	float3 tangent = normalize(inTangent);
	float3 bi_tangent = cross(tangent, normal);
	float3 tex_normal = cNormalTexture.Sample(cNormalSampler, inTexCoord).xyz * 2.0 - 1.0;
	tex_normal.y *= -1;
	float3x3 TBN = float3x3(tangent, bi_tangent, normal);
	return normalize(mul(tex_normal, TBN));	
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;
	output.LinearDepth = input.Position.z;
	output.Diffuse = cDiffuseTexture.Sample(cDiffuseSampler, input.TexCoord);
	
	output.Normal = EncodeNormal(NormalMap(input.Normal, input.Tangent, input.TexCoord));
	
	//float3 normal = PerturbNormal(input.Normal, input.Position.xyz, input.TexCoord);
	//output.Normal = EncodeNormal(input.Normal);

	output.Material = cMaterialTexture.Sample(cMaterialSampler, input.TexCoord);
	output.MotionVectors = input.MotionVectors;
	return output;
}
