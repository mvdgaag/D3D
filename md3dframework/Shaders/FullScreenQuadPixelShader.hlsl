Texture2D cTexture : register(t0);


struct PS_INPUT
{
	float4 Position	: SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};


SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	return cTexture.Sample(LinearSampler, input.TexCoord);
}
