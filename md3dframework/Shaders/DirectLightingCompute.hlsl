
RWTexture2D<float> dst : register(t0);
Texture2D<float> source : register(t1);


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
	float val = source.SampleLevel(LinearSampler, coord, 0);
	dst[coord] = val;
}
