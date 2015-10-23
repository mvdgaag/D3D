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


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output =		(PS_INPUT)0;

	// jitter for TAA
	float4x4 mvp = modelViewProjectionMatrix;
	float2 jitter_offset = frameData.xy;
	//mvp[2].xy += jitter_offset.xy;

	output.Position = mul(float4(input.Position, 1.0), mvp);
	output.Normal = mul(float4(input.Normal, 0.0), mvp).xyz;
	output.Tangent = mul(float4(input.Tangent, 0.0), mvp).xyz;
	output.TexCoord = input.TexCoord.xy;
	output.LinearDepth = mul(float4(input.Position, 1.0), (modelViewMatrix)).z;

	// motion vectors
	float4 prevPos = mul(float4(input.Position, 1.0), prevModelViewProjectionMatrix);
	output.MotionVectors = (output.Position.xy / output.Position.w); // currentNDC
	output.MotionVectors -= (prevPos.xy / prevPos.w); // prevNDC
	output.MotionVectors *= 0.5; // NDC to UV range conversion

	// jitter for TAA
	output.Position.xy += jitter_offset.xy * output.Position.w;
	//output.MotionVectors += 0.5 * jitter_offset.xy * output.Position.w;

	return output;
}