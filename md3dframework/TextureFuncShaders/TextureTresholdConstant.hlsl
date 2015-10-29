/* texture max function */
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source : register(t0);

cbuffer cConstantData : register(b0)
{
	float4 cTreshold;
}

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	dst[coord] = float4(source[coord] > cTreshold);
}
