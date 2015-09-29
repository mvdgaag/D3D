cbuffer cMatrices : register(b0)
{
	matrix View;
	matrix Projection;
	matrix WorldViewProjection;
	matrix MotionVectorMatrix;
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

	float4x4 VP = mul(View, Projection);

	output.Position = mul(float4(input.Position, 1.0), VP);
	output.Normal = mul(float4(input.Normal, 1.0), VP).xyz;
	output.Tangent = mul(float4(input.Tangent, 1.0), VP).xyz;
	output.TexCoord = input.TexCoord.xy;
	output.MotionVectors = mul(float4(input.Position, 1.0), MotionVectorMatrix).xy;
	
	/*output.Position =		mul(input.Position, WorldViewProjection);
	output.Normal =			mul(input.Normal, WorldViewProjection).xyz;
	output.Tangent =		mul(input.Tangent, WorldViewProjection).xyz;
	output.TexCoord =		input.TexCoord.xy;
	output.MotionVectors =	mul(input.Position, MotionVectorMatrix).xy;
*/
	return output;
}