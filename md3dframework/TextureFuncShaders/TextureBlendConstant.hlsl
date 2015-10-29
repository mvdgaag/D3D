/* texture max function */
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source1 : register(t0);
Texture2D<float4> source2 : register(t1);

cbuffer cConstantData : register(b0)
{
	float4 cOpacity;
}

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	dst[coord] = lerp(source1[coord], source2[coord], cOpacity);
}
