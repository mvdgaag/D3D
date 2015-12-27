/* texture mip nearest function */
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> src : register(t0);

cbuffer cConstantData : register(b0)
{
	float4 cParams;
}

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 dst_coord = int2(DTid.x, DTid.y);
	int3 src_coord = int3(DTid.x * 2, DTid.y * 2, cParams.x);
	dst[dst_coord] = src.Load(src_coord);
}
