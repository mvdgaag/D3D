RWTexture2D<float> rwFlux : register(u0);
Texture2D<float> tWaterHeight : register(t0);
Texture2D<float> tWaterDepth : register(t1);


float4 GetFlux(int2 inCoord)
{
	float4 result;
	inCoord.x *= 4;

	result.x = rwFlux[inCoord];
	inCoord.x += 1;
	result.y = rwFlux[inCoord];
	inCoord.x += 1;
	result.z = rwFlux[inCoord];
	inCoord.x += 1;
	result.w = rwFlux[inCoord];

	return result;
}


void SetFlux(int2 inCoord, float4 inFlux)
{
	inCoord.x *= 4;

	rwFlux[inCoord] = inFlux.x;
	inCoord.x += 1;
	rwFlux[inCoord] = inFlux.y;
	inCoord.x += 1;
	rwFlux[inCoord] = inFlux.z;
	inCoord.x += 1;
	rwFlux[inCoord] = inFlux.w;
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 coord = DTid.xy;

	float water_depth = tWaterDepth[coord];
	if (water_depth <= 0)
		return;

	float c = tWaterHeight[coord];
	float n = tWaterHeight[coord + int2(0, 1)];
	float e = tWaterHeight[coord + int2(1, 0)];
	float s = tWaterHeight[coord - int2(0, 1)];
	float w = tWaterHeight[coord - int2(1, 0)];

	const float l = 2.0;
	const float g = 9.81;
	const float dt = 0.01;
	const float A = 20.0;

	float k = l * g * dt * A;
	float fric = 0.999;

	float4 flux = GetFlux(coord);

	flux.x += max(0, k * (c - n));
	flux.y += max(0, k * (c - e));
	flux.z += max(0, k * (c - s));
	flux.w += max(0, k * (c - w));

	float sum_flux = dt * (flux.x + flux.y + flux.z + flux.w);
	if (sum_flux > water_depth)
		flux *= water_depth / sum_flux;

	flux *= fric;
	
	SetFlux(coord, flux);
}
