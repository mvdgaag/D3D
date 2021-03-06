cbuffer cEveryFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewProjectionMatrix;
	matrix inverseProjectionMatrix;
	float4 frameData;								// jitter_offset.xy, frameID, time
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
	float3 CameraPosition	: TEXCOORD3;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output =		(PS_INPUT)0;

	// jitter for TAA
	output.Position = mul(modelViewProjectionMatrix, float4(input.Position, 1.0));
	output.Normal = mul(modelViewMatrix, float4(input.Normal, 0.0)).xyz;
	output.Tangent = mul(modelViewMatrix, float4(input.Tangent, 0.0)).xyz;
	output.TexCoord = input.TexCoord.xy;

	float4 cam_space_pos = mul(modelViewMatrix, float4(input.Position, 1.0));
	output.CameraPosition = cam_space_pos.xyz;
	output.LinearDepth = -cam_space_pos.z;

	// motion vectors
	float4 prevPos = mul(prevModelViewProjectionMatrix, float4(input.Position, 1.0));
	output.MotionVectors = (output.Position.xy / output.Position.w); // currentNDC
	output.MotionVectors -= (prevPos.xy / prevPos.w); // prevNDC
	output.MotionVectors *= 0.5; // NDC to UV range conversion

	// jitter for TAA
	float2 jitter_offset = frameData.xy;
	output.Position.xy += jitter_offset.xy * output.Position.w;

	return output;
}