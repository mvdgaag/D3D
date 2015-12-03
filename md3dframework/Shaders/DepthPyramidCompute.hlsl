Texture2D<float> InputTexture			: register(t0);
RWTexture2D<float> OutputTextureMin		: register(u0); // 1/TILE_SIZE resolution
RWTexture2D<float> OutputTextureMax		: register(u0); // 1/TILE_SIZE resolution

#define TILE_SIZE	(8)
#define GROUP_SIZE	(TILE_SIZE * TILE_SIZE)

groupshared float sMinValue;
groupshared float sMaxValue;


[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void CS(uint3 inGroupID : SV_GroupID, uint3 inGroupThreadID : SV_GroupThreadID, uint inGroupIndex : SV_GroupIndex)
{
	int2 coord = inDispatchThreadID.xy;
	
	// first thread initializes shared memory
	if (inGroupIndex == 0)
	{
		sMinValue = sMaxValue = InputTexture[coord];
	}

	GroupMemoryBarrierWithGroupSync();

	InterLockedMin(sMinValue, InputTexture[coord]);
	InterLockedMax(sMaxValue, InputTexture[coord]);
	
	GroupMemoryBarrierWithGroupSync();

	OutputTextureMin = sMinValue;
	OutputTextureMax = sMaxValue;
}