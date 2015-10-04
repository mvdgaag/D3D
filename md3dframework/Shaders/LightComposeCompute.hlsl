
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> Direct : register(t0);
Texture2D<float4> Indirect : register(t1);
Texture2D<float4> Reflections : register(t2);


SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	dst[coord] = (Direct[coord] + Indirect[coord] + Reflections[coord]) / 3.0;
}
