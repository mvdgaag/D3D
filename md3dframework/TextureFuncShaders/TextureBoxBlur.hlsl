/* texture box blur function */
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source1 : register(t0);

cbuffer cConstantData : register(b0)
{
	float4 cParams;
}

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	int radius = cParams.x;
	int dir = cParams.y == 0 ? 0.0 : 1.0;
	int2 offset = int2(dir, 1.0 - dir);
	
	float4 result = float4(0.0, 0.0, 0.0, 0.0);
	int2 sample_coord = coord - offset * radius;
	for (int i = -radius; i < radius; i++, sample_coord += offset)
	{
		result += source1[sample_coord];
	}
	dst[coord] = 100 * result / (2 * radius);
}
