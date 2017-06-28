/* texture copy offset function */
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source : register(t0);

cbuffer cConstantData : register(b0)
{
	float4 cParams; // offset xy target size xy
}

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 source_coord = float2(DTid.x, DTid.y);
	float2 target_coord = source_coord + cParams.xy;

	if (target_coord.x < 0 || target_coord.y < 0 || target_coord.x >= cParams.z || target_coord.y >= cParams.w)
		return;

	dst[target_coord] = source[source_coord];
}
