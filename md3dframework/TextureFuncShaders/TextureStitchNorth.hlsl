/* texture stitch north function */
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source : register(t0);

cbuffer cConstantData : register(b0)
{
	float4 cParams; // x = texture width - 1, y = texture height - 1
}

[numthreads(64, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float3 coord = float3(DTid.x, cParams.x, 0);
	dst[coord.xy] = source[coord.xz];
}
