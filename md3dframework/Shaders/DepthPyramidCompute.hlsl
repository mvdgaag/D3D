Texture2D<float> InputTexture			: register(t0);
RWTexture2D<float> OutputTextureHalf	: register(u0); // 1/2 res (one thread per pixel)
RWTexture2D<float> OutputTextureQuarter : register(u1); // 1/4 res
RWTexture2D<float> OutputTextureEight	: register(u2); // 1/8 res

static const uint kNumThreads = 64;
groupshared float SharedMemHalf[kNumThreads];
groupshared float SharedMemQuarter[kNumThreads / 4];
groupshared float SharedMemEight[kNumThreads / 16];


[numthreads(8, 8, 1)]
void CS(uint3 GroupID : SV_GroupID, uint3 GroupThreadID : SV_GroupThreadID)
{
	const uint thread_idx = GroupThreadID.y * 8 + GroupThreadID.x;
	const uint2 sample_coord = (GroupID.xy * 8 + GroupThreadID.xy) * 2;

	float value;
	value = InputTexture[sample_coord + uint2(0, 0)];
	value += InputTexture[sample_coord + uint2(1, 0)];
	value += InputTexture[sample_coord + uint2(0, 1)];
	value += InputTexture[sample_coord + uint2(1, 1)];
	OutputTextureHalf[sample_coord / 2] = SharedMemHalf[thread_idx] = value * 0.25;

	SharedMemQuarter[thread_idx / 4] += SharedMemHalf[thread_idx]; // assume starts with zero
	GroupMemoryBarrierWithGroupSync();
	if (thread_idx % 4 == 0)
	{
		OutputTextureQuarter[sample_coord / 4] = SharedMemQuarter[thread_idx / 4] * (1.0 / 4.0);
		SharedMemEight[thread_idx / 16] += SharedMemQuarter[thread_idx / 4]; // assume starts with zero
	}

	GroupMemoryBarrierWithGroupSync();
	if (thread_idx % 16 == 0)
		OutputTextureEight[sample_coord / 8] = SharedMemEight[thread_idx / 16] * (1.0 / 16.0);
}