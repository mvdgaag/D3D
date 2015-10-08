RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source : register(t0);
Texture2D<float4> history : register(t1);
Texture2D<float2> motionVectors : register(t2);
SamplerState sourceSampler : register(s0);
SamplerState historySamper : register(s1);
SamplerState motionVectorSamper : register(s2);



[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	// current value
	float2 coord = float2(DTid.x, DTid.y);
	float4 val = source[coord];

	// history value
	float2 mv = motionVectors[coord];
	float2 history_coord = coord;
	float2 history_uv = (coord / float2(800, 600) -mv);
	float4 history_val = history.SampleLevel(historySamper, history_uv, 0);
	
	// neighborhood clamp
	float4 valn = source[coord + int2(0, 1)];
	float4 vals = source[coord + int2(1, 0)];
	float4 vale = source[coord - int2(0, 1)];
	float4 valw = source[coord - int2(1, 0)];
	float4 max_val = max(max(valn, vals), max(vale, valw));
	float4 min_val = min(min(valn, vals), min(vale, valw));
	history_val = clamp(history_val, min_val, max_val);

	// sharpen
	const float unsharp_strength = 4.0;
	val = (unsharp_strength + 1.0) * val - (unsharp_strength / 4.0) * (valn + vals + vale + valw);

	// blend
	const float blend_strength = 0.85;
	dst[coord] = lerp(val, history_val, blend_strength);
}
