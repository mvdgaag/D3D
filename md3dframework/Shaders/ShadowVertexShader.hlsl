cbuffer cObject : register(b0)
{
	matrix modelViewProjectionMatrix;
};


struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal	: NORMAL;	// TODO: remove
	float3 Tangent	: TANGENT;	// TODO: remove
	float2 TexCoord : TEXCOORD;	// TODO: remove
};


struct PS_INPUT
{
	float4 Position			: SV_POSITION;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output =		(PS_INPUT)0;

	output.Position = mul(float4(input.Position, 1.0), modelViewProjectionMatrix);
	
	return output;
}