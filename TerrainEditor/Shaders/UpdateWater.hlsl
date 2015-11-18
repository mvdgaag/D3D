RWTexture2D<float> rwWater : register(u0);
Texture2D<float> tTerrain : register(t0);
Texture2D<float> tFlux : register(t1);


float2 GetFlux(int2 inCoord)
{
	float2 result;
	inCoord.x *= 2;
	result.x = tFlux[inCoord];
	inCoord.x += 1;
	result.y = tFlux[inCoord];
	return result;
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 coord = DTid.xy;

	const float dt = 0.001;
	
	float water = rwWater[coord];
	float terrain = tTerrain[coord];
	
	float2 ne = GetFlux(coord);
	
	float2 sw;
	sw.x = GetFlux(coord - int2(1, 0)).x;
	sw.y = GetFlux(coord - int2(0, 1)).y;
	
	water += dt * ((ne.x - sw.x) + (ne.y - sw.y));
	water = max(0.0, water);
	
	rwWater[coord] = water;
}
