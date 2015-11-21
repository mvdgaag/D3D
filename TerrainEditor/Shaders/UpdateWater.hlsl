RWTexture2D<float> rwWaterDepth : register(u0);
RWTexture2D<float> rwWaterHeight : register(u1);
Texture2D<float> tTerrain : register(t0);
Texture2D<float> tFlux : register(t1);


cbuffer cConstantData : register(b0)
{
	float4 cParams;
	// x = pixel_height_scale (meters)
	// y = pixel_area (meter^2)
	// z = time_step
	// w = pixel_value / m^3
}


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

	// gather parameters
	const float height_scale = cParams.x;
	const float pixel_area = cParams.y;
	const float time_step = cParams.z;
	const float volume_scale = cParams.w;
	
	// the water depth and terrain height
	float water_depth = rwWaterDepth[coord];
	float terrain_height = tTerrain[coord];
	
	// the flux in and out of this texel
	float4 out_flux = GetFlux(coord);
	float4 in_flux;
	in_flux.x = GetFlux(coord + int2(0, 1)).z; // from north, to south
	in_flux.y = GetFlux(coord + int2(1, 0)).w; // from east, to west
	in_flux.z = GetFlux(coord - int2(0, 1)).x; // from south, to north
	in_flux.w = GetFlux(coord - int2(1, 0)).y; // from west, to east
	
	// the sum of all incoming and outgoing flux
	float sum_flux = (in_flux.x + in_flux.y + in_flux.z + in_flux.w) - (out_flux.x + out_flux.y + out_flux.z + out_flux.w);

	// update the water depth
	water_depth += time_step * volume_scale * sum_flux;
	water_depth += 0.001; // rain
	
	// cannot be negative
	water_depth = max(0.0, water_depth);
	
	rwWaterDepth[coord] = water_depth;
	rwWaterHeight[coord] = water_depth + terrain_height;
}
