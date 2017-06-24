RWTexture2D<float> rwFlux : register(u0);
Texture2D<float> tWaterHeight : register(t0);
Texture2D<float> tWaterDepth : register(t1);


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


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 tex_size;
	tWaterHeight.GetDimensions(tex_size.x, tex_size.y);

	// skip border pixels
	int2 coord = DTid.xy;

	if ((coord.x <= 0) || (coord.x >= tex_size.x - 1) || (coord.y <= 0) || (coord.y >= tex_size.y - 1))
		return;

	// only run the sim if there's water
	float water_depth = tWaterDepth[coord];
	if (water_depth <= 0)
		return;

	// get constants
	const float flux_constant = cParams.x;
	const float friction = cParams.y;
	const float time_step = cParams.z;
	const float volume_scale = cParams.w;

	// find the water height for this texel and its neighbors
	float c = tWaterHeight[coord];
	float n = tWaterHeight[coord + int2(0, 1)];
	float e = tWaterHeight[coord + int2(1, 0)];
	float s = tWaterHeight[coord - int2(0, 1)];
	float w = tWaterHeight[coord - int2(1, 0)];

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
