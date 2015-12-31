RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source : register(t0);


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	dst[coord] = source[coord];
}
