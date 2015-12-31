/* texture downsample function */
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source1 : register(t0);
SamplerState sampler1 : register(s0);

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);

	float2 target_size;
	dst.GetDimensions(target_size.x, target_size.y);
	
	float2 uv = coord / target_size;
	
	dst[coord] = source1.SampleLevel(sampler1, uv, 0);
}
