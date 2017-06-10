

// TODO: also add prevHeightmap for correct motion vectors
Texture2D cHeightTexture : register(t0);
SamplerState cHeightSampler : register(s0);

Texture2D cNormalTexture : register(t1);
SamplerState cNormalSampler : register(s1);


cbuffer cEveryFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewProjectionMatrix;
	matrix inverseProjectionMatrix;
	float4 frameData;								// jitter_offset.xy, frameID, 0
};

cbuffer cObject : register(b1)
{
	matrix modelViewMatrix;
	matrix modelViewProjectionMatrix;
	matrix prevModelViewProjectionMatrix;
};

cbuffer cTerrain : register(b2)
{
	float4 cTerrainScale;
};


struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent	: TANGENT;
	float2 TexCoord : TEXCOORD;
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



PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	
	float3 pos = input.Position;
	pos.y += cTerrainScale.z * cHeightTexture.SampleLevel(cHeightSampler, input.TexCoord, 0).x;
	
	/*
	float2 tex_size;
	cHeightTexture.GetDimensions(tex_size.x, tex_size.y);
	float2 rcp_tex_size = 1.0 / tex_size;
	
	float3 dx = float3(
		2.0 * rcp_tex_size.x,
		(cTerrainScale.z / cTerrainScale.x) *
		(	cHeightTexture.SampleLevel(cHeightSampler, saturate(input.TexCoord + float2( rcp_tex_size.x, 0.0)), 0).x -
			cHeightTexture.SampleLevel(cHeightSampler, saturate(input.TexCoord + float2(-rcp_tex_size.x, 0.0)), 0).x ),
		0.0);
	
	float3 dz = float3(
		0.0,
		(cTerrainScale.z / cTerrainScale.y) *
		(	cHeightTexture.SampleLevel(cHeightSampler, saturate(input.TexCoord + float2(0.0,  rcp_tex_size.y)), 0).x -
			cHeightTexture.SampleLevel(cHeightSampler, saturate(input.TexCoord + float2(0.0, -rcp_tex_size.y)), 0).x ),
		2.0 * rcp_tex_size.y);
	
	float3 nor = normalize(cross(dz, dx));
	output.Normal = mul(modelViewMatrix, float4(nor, 0.0)).xyz;
	*/

	float3 nor = cNormalTexture.SampleLevel(cNormalSampler, input.TexCoord, 0).xyz;// +float3(0, 1, 0);
	output.Normal = mul(modelViewMatrix, float4(nor, 0.0)).xyz; 

	// jitter for TAA
	output.Position = mul(modelViewProjectionMatrix, float4(pos, 1.0));
	output.Tangent = mul(modelViewMatrix, float4(input.Tangent, 0.0)).xyz;
	output.TexCoord = input.TexCoord.xy;

	float4 cam_space_pos = mul(modelViewMatrix, float4(pos, 1.0));
	output.LinearDepth = -cam_space_pos.z;
	
	// motion vectors
	float4 prevPos = mul(prevModelViewProjectionMatrix, float4(pos, 1.0));
	output.MotionVectors = (output.Position.xy / output.Position.w); // currentNDC
	output.MotionVectors -= (prevPos.xy / prevPos.w); // prevNDC
	output.MotionVectors *= 0.5; // NDC to UV range conversion
 
	// jitter for TAA
	float2 jitter_offset = frameData.xy;
	output.Position.xy += jitter_offset.xy * output.Position.w;

	return output;
}