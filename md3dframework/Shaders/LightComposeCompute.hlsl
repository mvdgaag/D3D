
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
	// coord from bottom left of brush rect
	float2 coord = float2(DTid.x, DTid.y);

		/*
	float4 indirect = 0;
	for (int x = -2; x < 2; x++)
	{
	for (int y = -2; y < 2; y++)
	{
	indirect += Indirect[coord + int2(x, y)];
	}
	}

	indirect /= 16.0;
	*/

	dst[coord] = Indirect[coord] + DirectDiffuse[coord] + DirectSpecular[coord] + Reflections[coord];
}
