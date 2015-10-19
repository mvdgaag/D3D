
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source : register(t0);
Texture2D<float2> motionVectors : register(t1);
SamplerState sourceSampler : register(s0);
SamplerState motionSampler : register(s1);


cbuffer cPostProcessConstants : register(b0)
{
	float2	cTargetSize;
};


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	float2 uv = (coord + 0.5) / cTargetSize;

	float2 mv = motionVectors.SampleLevel(motionSampler, uv, 0);
	// TODO: compensate for jitter? Do so in base fragment shader instead?
	float2 history_uv = saturate(uv - mv);

	float4 result;
	for (int i = 0; i < 8; i++)
	{
		float2 samp_uv = lerp(uv, history_uv, float(i) / 8.0);
		result += source.SampleLevel(sourceSampler, samp_uv, 0);
	}

	dst[coord] = result / 8.0;
	//dst[coord] = source[coord];
}
