cbuffer cMatrices : register(b0)
{
	matrix MVP;
	matrix prevMVP;
	float4 offsets; // .xy = halton offset for this frame, .zw for the difference for the motion vectors
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
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output =		(PS_INPUT)0;

	output.Position = mul(float4(input.Position, 1.0), MVP);
	output.Normal = mul(float4(input.Normal, 1.0), MVP).xyz;
	output.Tangent = mul(float4(input.Tangent, 1.0), MVP).xyz;
	output.TexCoord = input.TexCoord.xy;

	// motion vectors
	float4 prevPos = mul(float4(input.Position, 1.0), prevMVP);
	output.MotionVectors = (output.Position.xy / output.Position.w); // currentNDC
	output.MotionVectors -= (prevPos.xy / prevPos.w); // prevNDC
	output.MotionVectors *= 0.5; // NDC to UV range conversion

	// jitter for TAA
	//output.Position.xy += offsets.xy;

	return output;
}