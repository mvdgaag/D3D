RWTexture2D<float> rwFlux :				register(u0);
Texture2D<float> tWaterHeight :			register(t0);
Texture2D<float> tWaterDepth :			register(t1);
Texture2D<float> tWaterHeightNorth :	register(t2);
Texture2D<float> tWaterHeightEast :		register(t3);
Texture2D<float> tFluxSouth :			register(t4);
Texture2D<float> tFluxWest :			register(t5);


cbuffer cConstantData : register(b0)
{
	float4 cParams; 
	// x = time_step * cell_size * gravity * fluidity * pixel_height_scale / pixel_width_scale
	// y = friction coefficient
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


float4 GetFlux(RWTexture2D<float> inTexture, int2 inCoord)
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


[numthreads(32, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 tex_size;
	tWaterHeight.GetDimensions(tex_size.x, tex_size.y);

	int2 coord1 = int2(DTid.x, tex_size.y - 1);
	int2 coord2 = int2(tex_size.x - 1, DTid.x);

	// only run the sim if there's water
	float water_depth1 = tWaterDepth[coord1];
	float water_depth2 = tWaterDepth[coord2];

	// find the water height for this texel and its neighbors
	float c1 = tWaterHeight[coord1];
	float n1 = tWaterHeight[coord1 + int2(0, 1)];
	float e1 = tWaterHeight[coord1 + int2(1, 0)];
	float s1 = tWaterHeightNorth[int2(coord1.x, 0)]; // -2 because border pixel (tex_size - 1) is overlapping;
	float w1 = tWaterHeight[coord1 - int2(1, 0)];
	float c2 = tWaterHeight[coord2];
	float n2 = tWaterHeight[coord2 + int2(0, 1)];
	float e2 = tWaterHeight[coord2 + int2(1, 0)];
	float s2 = tWaterHeight[coord2 - int2(0, 1)];
	float w2 = tWaterHeightEast[int2(0, coord2.y)];

	// get constants
	const float flux_constant = cParams.x;
	const float friction = cParams.y;
	const float time_step = cParams.z;
	const float volume_scale = cParams.w;

	// get the current flux
	float4 flux1 = GetFlux(rwFlux, coord1);
	float4 flux2 = GetFlux(rwFlux, coord2);

	// we're only keeping track of flux out of the cell. Update
	flux1.x += max(0, flux_constant * (c1 - n1));
	flux1.y += max(0, flux_constant * (c1 - e1));
	flux1.z += max(0, flux_constant * (c1 - s1));
	flux1.w += max(0, flux_constant * (c1 - w1));
	flux2.x += max(0, flux_constant * (c2 - n2));
	flux2.y += max(0, flux_constant * (c2 - e2));
	flux2.z += max(0, flux_constant * (c2 - s2));
	flux2.w += max(0, flux_constant * (c2 - w2));

	// if the height difference this causes is larger then the depth, scale the flux accordingly
	float delta_height1 = time_step * volume_scale * (flux1.x + flux1.y + flux1.z + flux1.w);
	if (delta_height1 > water_depth1)
		flux1 *= water_depth1 / delta_height1;
	float delta_height2 = time_step * volume_scale * (flux2.x + flux2.y + flux2.z + flux2.w);
	if (delta_height2 > water_depth2)
		flux2 *= water_depth2 / delta_height2;

	// friction based on the wave height per water depth
	float fric1 = friction * water_depth1 / (water_depth1 + delta_height1);
	flux1 *= pow(abs(fric1), time_step);
	float fric2 = friction * water_depth2 / (water_depth2 + delta_height2);
	flux2 *= pow(abs(fric2), time_step);

	// set north east borders
	SetFlux(coord1, flux1);
	SetFlux(coord2, flux2);

	// copy south west borders
	SetFlux(int2(DTid.x, 0), GetFlux(tFluxSouth, coord1));
	SetFlux(int2(0, DTid.x), GetFlux(tFluxWest, coord2));
}
