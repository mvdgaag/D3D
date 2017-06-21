#include "LibHelper.hlsli"


Texture2D cDiffuseTexture : register(t0);
Texture2D cNormalTexture : register(t1);
Texture2D cSurfaceTexture : register(t2);

SamplerState cDiffuseSampler : register(s0);
SamplerState cNormalSampler : register(s1);
SamplerState cSurfaceSampler : register(s2);


#define HAS_DIFFUSE_MAP	(1 << 0)
#define HAS_NORMAL_MAP	(1 << 1)
#define HAS_SURFACE_MAP	(1 << 2)


cbuffer cConstantData : register(b0)
{
	float4 cDiffuse;
	float4 cSurface;
	uint cFlags;
	float4 cPadding;
}


struct PS_INPUT
{
	float4 Position			: SV_POSITION;
	float3 Normal			: NORMAL;
	float3 Tangent			: TANGENT;
	float2 TexCoord			: TEXCOORD0;
	float2 MotionVectors	: TEXCOORD1;
	float  LinearDepth		: TEXCOORD2;
	float3 CameraPosition	: TEXCOORD3;
};


struct PS_OUTPUT
{
	float LinearDepth		: SV_Target0;
	half2 Normal			: SV_Target1;
	float4 Diffuse			: SV_Target2;
	float4 Surface			: SV_Target3;
	half2 MotionVectors		: SV_Target4;
};


// returns a new F0. Approximates occlusion for a cone reflected from the normal by the surface described by the geometry normal
float ReflectionOcclusion(float3 inView, float3 inNormal, float3 inGeomNormal, float inF0, float inRoughness)
{
	float3 r = reflect(inView, inNormal);
	float x = saturate(0.5 * (dot(r, inGeomNormal) / inRoughness) + 0.5);
	return lerp(inF0 * inF0, inF0, x);
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
	output.LinearDepth = input.LinearDepth;
	
	uint flags = cFlags;

	if ((flags & HAS_DIFFUSE_MAP) == 0)
		output.Diffuse = cDiffuse;
	else
		output.Diffuse = cDiffuseTexture.Sample(cDiffuseSampler, input.TexCoord);

	if ((flags & HAS_SURFACE_MAP) == 0)
		output.Surface = cSurface;
	else
		output.Surface = cSurfaceTexture.Sample(cSurfaceSampler, input.TexCoord);

	if ((flags & HAS_NORMAL_MAP) == 0)
		output.Normal = EncodeNormal(normalize(input.Normal));
	else
	{
		float fresnel0 = output.Surface.y;
		float roughness = output.Surface.x;
		float3 normal = NormalMap(normalize(input.Normal), input.Tangent, input.TexCoord);
		float3 view = normalize(input.CameraPosition);
		
		output.Surface.y = ReflectionOcclusion(view, normal, input.Normal, fresnel0, roughness);
		output.Normal = EncodeNormal(normal);
	}

	output.MotionVectors = input.MotionVectors;

	return output;
}
