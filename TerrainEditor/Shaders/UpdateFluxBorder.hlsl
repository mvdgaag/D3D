RWTexture2D<float> rwFlux :				register(u0);
Texture2D<float> tWaterHeight :			register(t0);
Texture2D<float> tWaterDepth :			register(t1);
Texture2D<float> tWaterHeightNorth :	register(t2);
Texture2D<float> tWaterHeightEast :		register(t3);
Texture2D<float> tWaterHeightSouth :	register(t4);
Texture2D<float> tWaterHeightWest :		register(t5);


cbuffer cConstantData : register(b0)
{
	float4 cParams; 
	// x = time_step * cell_size * gravity * fluidity * pixel_height_scale / pixel_width_scale
	// y = friction coefficient
	// z = time_step
	// w = pixel_value / m^3
}


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


[numthreads(32, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	// get constants
	int2 tex_size;
	tWaterHeight.GetDimensions(tex_size.x, tex_size.y);
	const float flux_constant = cParams.x;
	const float friction = cParams.y;
	const float time_step = cParams.z;
	const float volume_scale = cParams.w;

	int select = DTid.y;

	int2 coord;
	if (select == 0)		coord = int2(DTid.x, tex_size.y - 1);
	else if (select == 1)	coord = int2(tex_size.x - 1, DTid.x);
	else if (select == 2)	coord = int2(DTid.x, 0);
	else					coord = int2(0, DTid.x);

	// only run the sim if there's water
	float water_depth = tWaterDepth[coord];
	if (water_depth <= 0)
		return;

	float c = tWaterHeight[coord];
	float n = (coord.y == tex_size.y-1) ?	tWaterHeightNorth[int2(coord.x, 1)]					: tWaterHeight[coord + int2(0, 1)];
	float e = (coord.x == tex_size.x-1) ?	tWaterHeightEast[ int2(1, coord.y)]					: tWaterHeight[coord + int2(1, 0)];
	float s = (coord.y == 0) ?				tWaterHeightSouth[int2(coord.x, tex_size.y - 2)]	: tWaterHeight[coord - int2(0, 1)];
	float w = (coord.x == 0) ?				tWaterHeightWest[ int2(tex_size.x - 2, coord.y)]	: tWaterHeight[coord - int2(1, 0)];

	// get the current flux
	float4 flux = GetFlux(coord);

	// we're only keeping track of flux out of the cell.
	// update according to height differences
	flux.x += max(0, flux_constant * (c - n));
	flux.y += max(0, flux_constant * (c - e));
	flux.z += max(0, flux_constant * (c - s));
	flux.w += max(0, flux_constant * (c - w));

	// if the height difference this causes is larger then the depth, scale the flux accordingly
	float delta_height = time_step * volume_scale * (flux.x + flux.y + flux.z + flux.w);
	if (delta_height > water_depth)
		flux *= water_depth / delta_height;

	// friction based on the wave height per water depth
	// note that delta height is time dependent again
	float fric = friction * saturate(3 * water_depth / (water_depth + delta_height / time_step));

	// friction
	flux *= pow(fric, time_step);

	SetFlux(coord, flux);
}
