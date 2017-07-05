#include "../../md3dframework/Shaders/LibHelper.hlsli"

Texture2D cDiffuseTexture : register(t0);
Texture2D cNormalTexture : register(t1);
Texture2D cSurfaceTexture : register(t2);

SamplerState cDiffuseSampler : register(s0);
SamplerState cNormalSampler : register(s1);
SamplerState cSurfaceSampler : register(s2);


#define HAS_DIFFUSE_MAP	(1 << 0)
#define HAS_NORMAL_MAP	(1 << 1)
#define HAS_SURFACE_MAP	(1 << 2)


cbuffer cEveryFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewProjectionMatrix;
	matrix inverseProjectionMatrix;
	float4 frameData;								// jitter_offset.xy, frameID, time
};


cbuffer cConstantData : register(b1)
{
	float4 cDiffuse;
	float4 cSurface;
	uint cFlags;
	float4 cPadding;
};


struct PS_INPUT
{
	float4 Position			: SV_POSITION;
	float3 Normal			: NORMAL;
	float3 Tangent			: TANGENT;
	float2 TexCoord			: TEXCOORD0;
	float2 MotionVectors	: TEXCOORD1;
	float  LinearDepth		: TEXCOORD2;
};


struct PS_OUTPUT
{
	float LinearDepth		: SV_Target0;
	half2 Normal			: SV_Target1;
	float4 Diffuse			: SV_Target2;
	float4 Surface			: SV_Target3;
	half2 MotionVectors		: SV_Target4;
};

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

	// terrain always needs normalmap
	output.Normal = EncodeNormal(input.Normal.xyz);

	if ((flags & HAS_SURFACE_MAP) == 0)
		output.Surface = cSurface;
	else
		output.Surface = cSurfaceTexture.Sample(cSurfaceSampler, input.TexCoord);
	
	output.MotionVectors = input.MotionVectors;


	// DEVHACK
	//output.Diffuse = float4(input.TexCoord, 0.0, 1.0);

	return output;
}
