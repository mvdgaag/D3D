RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source : register(t0);
Texture2D<float4> history : register(t1);
Texture2D<float2> motionVectors : register(t2);
SamplerState sourceSampler : register(s0);
SamplerState historySamper : register(s1);
SamplerState motionVectorSamper : register(s2);


cbuffer cTXAAConstants : register(b0)
{
	float2	cJitterOffset;
	float2	cTargetSize;
};


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	// current value
	float2 coord = float2(DTid.x, DTid.y);
	float2 counter_jitter = float2(0.5, -0.5) * cJitterOffset;
	float2 current_uv = (coord + 0.5) / cTargetSize + counter_jitter;
	float4 val = source.SampleLevel(sourceSampler, current_uv, 0);

	// history value
	float2 mv = motionVectors.SampleLevel(motionVectorSamper, current_uv, 0);
	float2 history_uv = (coord + 0.5) / cTargetSize - mv;
	float4 history_val = history.SampleLevel(historySamper, history_uv, 0);
	
	// sharpen new value
	float4 valn = source[coord + int2(0, 1)];
	float4 vals = source[coord + int2(1, 0)];
	float4 vale = source[coord - int2(0, 1)];
	float4 valw = source[coord - int2(1, 0)];
	const float unsharp_strength = 4.0;
	val = (unsharp_strength + 1.0) * val - (unsharp_strength * 0.25) * (valn + vals + vale + valw);

	// neighborhood clamp
	float4 max_val = max(max(valn, vals), max(vale, valw));
	float4 min_val = min(min(valn, vals), min(vale, valw));
	history_val = clamp(history_val, min_val, max_val);

	// blend
	const float blend_strength = 0.95;
	dst[coord] = lerp(val, history_val, blend_strength);
}
