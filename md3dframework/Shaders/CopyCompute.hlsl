
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> sourceTexture : register(t0);
SamplerState sourceSampler : register(s0);

cbuffer cConstants : register(b0)
{
	float4 cData; // width, height, miplevels, 0
};

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	float2 uv = (coord + 0.5) / cData.xy;
	dst[coord] = sourceTexture.SampleLevel(sourceSampler, uv, uint(cData.z));
}
