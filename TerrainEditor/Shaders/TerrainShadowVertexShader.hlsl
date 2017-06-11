Texture2D cHeightTexture : register(t0);
SamplerState cHeightSampler : register(s0);

cbuffer cObject : register(b0)
{
	matrix modelViewProjectionMatrix;
};

cbuffer cTerrain : register(b1)
{
	float4 cTerrainScale;
};


struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal	: NORMAL;	// TODO: remove
	float3 Tangent	: TANGENT;	// TODO: remove
	float2 TexCoord : TEXCOORD;	// Assumes vertex coordinate instead of UV
};


struct PS_INPUT
{
	float4 Position			: SV_POSITION;
};



PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	int3 pixel = int3(int2(input.TexCoord), 0);
	float3 pos = input.Position;
	pos.y += cTerrainScale.z * cHeightTexture.Load(pixel).x;

	output.Position = mul(float4(pos, 1.0), modelViewProjectionMatrix);

	return output;
}