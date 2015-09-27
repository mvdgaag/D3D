struct VS_INPUT
{
	float3 Position : POSITION;
};


struct PS_INPUT
{
	float4 Position			: SV_POSITION;
	float2 TexCoord			: TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output =		(PS_INPUT)0;
	output.Position =		float4(input.Position, 1);
	output.TexCoord =		input.Position.xy * 0.5 + 0.5;
	return output;
}