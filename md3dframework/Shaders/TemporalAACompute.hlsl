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


float ApproximateLuma(float3 inRGB)
{
	return sqrt(0.299 * inRGB.x * inRGB.x + 0.587 * inRGB.y * inRGB.y + 0.114 * inRGB.z * inRGB.z);
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	// current value
	float2 coord = float2(DTid.x, DTid.y);
	float2 counter_jitter = float2(0.5, -0.5) * cJitterOffset;
	float2 current_uv = (coord + 0.5) / cTargetSize + counter_jitter;
	float4 val = source.SampleLevel(sourceSampler, current_uv, 0);

	// history value (stupid dx11 doesn't allow float4 writes to RWTexture2D, so use history texture)
	float2 mv = motionVectors.SampleLevel(motionVectorSamper, current_uv, 0) * float2(1, -1);
	float2 uv = (coord + 0.5) / cTargetSize;
	float2 history_uv = uv - mv;
	float4 history_val = history.SampleLevel(historySamper, history_uv, 0);
	
	if (isnan(history_val.x) == true)
		history_val = 0;

	// gather neighborhood
	float4 valn = source[coord + int2(0, 1)];
	float4 valne = source[coord + int2(-1, 1)];
	float4 vale = source[coord + int2(1, 0)];
	float4 valse = source[coord + int2(-1, -1)];
	float4 vals = source[coord - int2(0, 1)];
	float4 valsw = source[coord + int2(1, -1)];
	float4 valw = source[coord - int2(1, 0)];
	float4 valnw = source[coord + int2( 1, 1)];
	
	// sharpen new value
	// TODO: does not work with high contrast. disabled for now
	const float unsharp_strength = 0.0;
	const float normalization_factor = 8.0 / (4.0 + 4.0 / sqrt(2));
	val = (unsharp_strength + 1.0) * val - 
		(unsharp_strength * normalization_factor * 0.125) * (valn + vale + vals + valw) -
		(unsharp_strength * normalization_factor * (0.125 / sqrt(2.0))) * (valne + valse + valnw + valsw);

	// base blend factor on coherence
	float4 color_coherence = max(0.8, val / (val + 2.0 * abs(val - history_val)));

	// 3*3 neighborhood clamp
	float4 max_val = max(max(max(valn, vals), max(vale, valw)), max(max(valne, valse), max(valnw, valsw)));
	float4 min_val = min(min(min(valn, vals), min(vale, valw)), min(min(valne, valse), min(valnw, valsw)));

	// give some room for noise supression if the reprojected value is within the neighborhood range
	// the amount is based on the amount of color coherence
	max_val += (val * color_coherence < max_val) * 0.05 * color_coherence;
	min_val -= (val > color_coherence * min_val) * 0.05 * color_coherence;
	
	// clamp
	history_val = clamp(history_val, min_val, max_val);
	
	val = saturate(val);
	history_val = saturate(history_val);

	// blend
	dst[coord] = saturate(lerp(val, history_val, color_coherence));
}
