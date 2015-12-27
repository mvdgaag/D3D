/* texture mip max function */
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
	float4 result = src.Load(src_coord);
	result = max(result, src.Load(src_coord + int3(1, 0, 0)));
	result = max(result, src.Load(src_coord + int3(0, 1, 0)));
	result = max(result, src.Load(src_coord + int3(1, 1, 0)));
	dst[dst_coord] = result;
}
