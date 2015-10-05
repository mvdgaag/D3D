
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source : register(t0);
Texture2D<float4> history : register(t1);
Texture2D<float2> motionVectors : register(t2);

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
	float4 val = source[coord];

	float2 mv = motionVectors[coord];

	// try filtered
	float2 history_coord = (coord - mv) / float2(800, 600);
	float4 history_val = history.SampleLevel(LinearSampler, history_coord, 0);
	
	dst[coord] = lerp(val, history_val, 0.9);
}
