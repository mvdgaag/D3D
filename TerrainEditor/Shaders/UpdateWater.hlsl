RWTexture2D<float> rwWaterDepth : register(u0);
RWTexture2D<float> rwWaterHeight : register(u1);
Texture2D<float> tTerrain : register(t0);
Texture2D<float> tFlux : register(t1);


float4 GetFlux(int2 inCoord)
{
	float4 result;
	inCoord.x *= 4;
	result.x = tFlux[inCoord];
	inCoord.x += 1;
	result.y = tFlux[inCoord];
	inCoord.x += 1;
	result.z = tFlux[inCoord];
	inCoord.x += 1;
	result.w = tFlux[inCoord];
	return result;
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 coord = DTid.xy;

	const float dt = 0.01;
	
	float water_depth = rwWaterDepth[coord];
	float terrain_height = tTerrain[coord];
	
	float4 out_flux = GetFlux(coord);
	
	float4 in_flux;
	in_flux.x = GetFlux(coord + int2(0, 1)).z; // from north, to south
	in_flux.y = GetFlux(coord + int2(1, 0)).w; // from east, to west
	in_flux.z = GetFlux(coord - int2(0, 1)).x; // from south, to north
	in_flux.w = GetFlux(coord - int2(1, 0)).y; // from west, to east
	
	float sum_flux = (in_flux.x + in_flux.y + in_flux.z + in_flux.w) - (out_flux.x + out_flux.y + out_flux.z + out_flux.w);

	water_depth += dt * sum_flux;
	water_depth += 0.001; // rain
	water_depth = max(0.0, water_depth);
	
	rwWaterDepth[coord] = water_depth;
	rwWaterHeight[coord] = water_depth + terrain_height;
}