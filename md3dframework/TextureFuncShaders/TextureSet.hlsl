/* texture max function */
RWTexture2D<float4> dst : register(u0);

cbuffer cConstantData : register(b0)
{
	float4 cVal;
}

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	dst[coord] = cVal;
}
