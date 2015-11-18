RWTexture2D<float> rwFlux : register(u0);
Texture2D<float> tTerrain : register(t0);
Texture2D<float> tWater : register(t1);


float2 GetFlux(int2 inCoord)
{
	float2 result;
	inCoord.x *= 2;
	result.x = rwFlux[inCoord];
	inCoord.x += 1;
	result.y = rwFlux[inCoord];
	return result;
}


void SetFlux(int2 inCoord, float2 inFlux)
{
	inCoord.x *= 2;
	rwFlux[inCoord] = inFlux.x;
	inCoord.x += 1;
	rwFlux[inCoord] = inFlux.y;
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 coord = DTid.xy;

	float c = tWater[coord] + tTerrain[coord];
	float n = tWater[coord + int2( 0, 1)] + tTerrain[coord + int2( 0, 1)];
	float e = tWater[coord + int2( 1, 0)] + tTerrain[coord + int2( 1, 0)];

	const float l = 2.0;
	const float g = 9.81;
	const float dt = 0.001;
	const float A = 10.0;

	float k = l * g * dt * A;
	float fric = 0.99;

	float2 flux = GetFlux(coord);

	flux += k * float2(e - c, n - c);
	flux *= fric;
	
	SetFlux(coord, flux);
}
