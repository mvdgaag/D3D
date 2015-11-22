RWTexture2D<float> rwWaterDepth :	register(u0);
RWTexture2D<float> rwWaterHeight :	register(u1);
Texture2D<float> tTerrain :			register(t0);

Texture2D<float> tFlux :			register(t1);
Texture2D<float> tFluxNorth :		register(t2);
Texture2D<float> tFluxEast :		register(t3);

Texture2D<float> tWaterDepthSouth :		register(t4);
Texture2D<float> tWaterDepthWest :		register(t5);
Texture2D<float> tWaterHeightSouth :	register(t6);
Texture2D<float> tWaterHeightWest :		register(t7);


cbuffer cConstantData : register(b0)
{
	float4 cParams;
	// x = pixel_height_scale (meters)
	// y = pixel_area (meter^2)
	// z = time_step
	// w = pixel_value / m^3
}


float4 GetFlux(Texture2D<float> inTexture, int2 inCoord)
{
	float4 result;
	inCoord.x *= 4;
	result.x = inTexture[inCoord];
	inCoord.x += 1;
	result.y = inTexture[inCoord];
	inCoord.x += 1;
	result.z = inTexture[inCoord];
	inCoord.x += 1;
	result.w = inTexture[inCoord];
	return result;
}


[numthreads(32, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 tex_size;
	rwWaterHeight.GetDimensions(tex_size.x, tex_size.y);

	int2 coord1 = int2(DTid.x, tex_size.y - 1);
	int2 coord2 = int2(tex_size.x - 1, DTid.x);

	// gather parameters
	const float height_scale = cParams.x;
	const float pixel_area = cParams.y;
	const float time_step = cParams.z;
	const float volume_scale = cParams.w;
	
	// the water depth and terrain height
	float water_depth1 = rwWaterDepth[coord1];
	float terrain_height1 = tTerrain[coord1];
	float water_depth2 = rwWaterDepth[coord2];
	float terrain_height2 = tTerrain[coord2];

	// the flux in and out of this texel
	float4 out_flux1 = GetFlux(tFlux, coord1);
	float4 in_flux1;
	in_flux1.x = GetFlux(tFlux, coord1 + int2(0, 1)).z; // from north, to south
	in_flux1.y = GetFlux(tFlux, coord1 + int2(1, 0)).w; // from east, to west
	in_flux1.z = GetFlux(tFluxNorth, int2(coord1.x, 0)).x; // from south, to north
	in_flux1.w = GetFlux(tFlux, coord1 - int2(1, 0)).y; // from west, to east
	float4 out_flux2 = GetFlux(tFlux, coord2);
	float4 in_flux2;
	in_flux2.x = GetFlux(tFlux, coord2 + int2(0, 1)).z; // from north, to south
	in_flux2.y = GetFlux(tFlux, coord2 + int2(1, 0)).w; // from east, to west
	in_flux2.z = GetFlux(tFlux, coord2 - int2(0, 1)).x; // from south, to north
	in_flux2.w = GetFlux(tFluxEast, int2(0, coord2.y)).y; // from west, to east

	// the sum of all incoming and outgoing flux
	float sum_flux1 = (in_flux1.x + in_flux1.y + in_flux1.z + in_flux1.w) - (out_flux1.x + out_flux1.y + out_flux1.z + out_flux1.w);
	float sum_flux2 = (in_flux2.x + in_flux2.y + in_flux2.z + in_flux2.w) - (out_flux2.x + out_flux2.y + out_flux2.z + out_flux2.w);

	// update the water depth
	water_depth1 += time_step * volume_scale * sum_flux1;
	water_depth1 += 0.001; // rain
	water_depth2 += time_step * volume_scale * sum_flux2;
	water_depth2 += 0.001; // rain

	// cannot be negative
	water_depth1 = max(0.0, water_depth1);
	water_depth2 = max(0.0, water_depth2);
	
	// set north east borders
	rwWaterDepth[coord1] = water_depth1;
	rwWaterHeight[coord1] = water_depth1 + terrain_height1;
	rwWaterDepth[coord2] = water_depth2;
	rwWaterHeight[coord2] = water_depth2 + terrain_height2;

	// copy south west borders
	rwWaterDepth[int2(DTid.x, 0)] = tWaterDepthSouth[coord1];
	rwWaterHeight[int2(DTid.x, 0)] = tWaterHeightSouth[coord1];
	rwWaterDepth[int2(0, DTid.x)] = tWaterDepthWest[coord2];
	rwWaterHeight[int2(0, DTid.x)] = tWaterHeightWest[coord2];
}
