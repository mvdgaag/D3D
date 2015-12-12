
RWTexture2D<float4> dst : register(u0);
Texture2D<float4> source : register(t0);
Texture2D<float2> motionVectors : register(t1);
SamplerState sourceSampler : register(s0);
SamplerState motionSampler : register(s1);


#define MOTION_BLUR_STRENGTH (1.0)
#define MOTION_BLUR_STEPS (12)
#define GAMMA (1.0)


cbuffer cPostProcessConstants : register(b0)
{
	float2	cTargetSize;
};


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	float2 uv = (coord + 0.5) / cTargetSize;
	float2 mv = motionVectors.SampleLevel(motionSampler, uv, 0) * float2(1, -1);
	float2 history_uv = saturate(uv + mv * MOTION_BLUR_STRENGTH);

	float4 result = 0;
	for (int i = 0; i < MOTION_BLUR_STEPS; i++)
	{
		float2 samp_uv = lerp(uv, history_uv, float(i) / MOTION_BLUR_STEPS);
		result += source.SampleLevel(sourceSampler, samp_uv, 0);
	}

	dst[coord] = pow(result / MOTION_BLUR_STEPS, GAMMA);
	//dst[coord] = source[coord];
}
