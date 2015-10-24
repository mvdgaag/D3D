
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> DirectDiffuse : register(t0);
Texture2D<float4> DirectSpecular : register(t1);
Texture2D<float4> Indirect : register(t2);
Texture2D<float4> Reflections : register(t3);


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
	dst[coord] = DirectDiffuse[coord] + DirectSpecular[coord] + Indirect[coord] + Reflections[coord];
}
