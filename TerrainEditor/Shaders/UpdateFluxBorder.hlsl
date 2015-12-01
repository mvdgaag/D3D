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
	// get constants
	int2 tex_size;
	tWaterHeight.GetDimensions(tex_size.x, tex_size.y);
	const float flux_constant = cParams.x;
	const float friction = cParams.y;
	const float time_step = cParams.z;
	const float volume_scale = cParams.w;

	// define coordinates (we're processing 4 texels at once)
	int2 coord_north_edge = int2(DTid.x, tex_size.y - 1);
	int2 coord_east_edge =	int2(tex_size.x - 1, DTid.x);
	int2 coord_south_edge = int2(DTid.x, 0);
	int2 coord_west_edge =	int2(0, DTid.x);

	// for the north and east edges, find the water height for this texel and its neighbors
	float c1 = tWaterHeight[coord_north_edge];
	float n1 = tWaterHeightNorth[coord_south_edge];
	float e1 = tWaterHeight[coord_north_edge + int2(1, 0)];
	float s1 = tWaterHeight[coord_north_edge - int2(0, 1)];
	float w1 = tWaterHeight[coord_north_edge - int2(1, 0)];

	float c2 = tWaterHeight[coord_east_edge];
	float n2 = tWaterHeight[coord_east_edge + int2(0, 1)];
	float e2 = tWaterHeightEast[coord_west_edge];
	float s2 = tWaterHeight[coord_east_edge - int2(0, 1)];
	float w2 = tWaterHeight[coord_east_edge - int2(1, 0)];

	float c3 = tWaterHeight[coord_south_edge];
	float n3 = tWaterHeight[coord_south_edge + int2(0, 1)];
	float e3 = tWaterHeight[coord_south_edge - int2(1, 0)];
	float s3 = tWaterHeightSouth[coord_north_edge];
	float w3 = tWaterHeight[coord_south_edge - int2(1, 0)];

	float c4 = tWaterHeight[coord_west_edge];
	float n4 = tWaterHeight[coord_west_edge + int2(0, 1)];
	float e4 = tWaterHeight[coord_west_edge + int2(1, 0)];
	float s4 = tWaterHeight[coord_west_edge - int2(0, 1)];
	float w4 = tWaterHeightWest[coord_east_edge];

	// get the current flux
	float4 flux_north_edge =	GetFlux(rwFlux, coord_north_edge);
	float4 flux_east_edge =		GetFlux(rwFlux, coord_east_edge);
	float4 flux_south_edge =	GetFlux(rwFlux, coord_south_edge);
	float4 flux_west_edge =		GetFlux(rwFlux, coord_west_edge);

	// we're only keeping track of flux out of the cell.
	flux_north_edge.x += max(0, flux_constant * (c1 - n1));
	flux_north_edge.y += max(0, flux_constant * (c1 - e1));
	flux_north_edge.z += max(0, flux_constant * (c1 - s1));
	flux_north_edge.w += max(0, flux_constant * (c1 - w1));

	flux_east_edge.x += max(0, flux_constant * (c2 - n2));
	flux_east_edge.y += max(0, flux_constant * (c2 - e2));
	flux_east_edge.z += max(0, flux_constant * (c2 - s2));
	flux_east_edge.w += max(0, flux_constant * (c2 - w2));

	flux_south_edge.x += max(0, flux_constant * (c3 - n3));
	flux_south_edge.y += max(0, flux_constant * (c3 - e3));
	flux_south_edge.z += max(0, flux_constant * (c3 - s3));
	flux_south_edge.w += max(0, flux_constant * (c3 - w3));

	flux_west_edge.x += max(0, flux_constant * (c4 - n4));
	flux_west_edge.y += max(0, flux_constant * (c4 - e4));
	flux_west_edge.z += max(0, flux_constant * (c4 - s4));
	flux_west_edge.w += max(0, flux_constant * (c4 - w4));

	// get the water depth
	float water_depth_north_edge =	tWaterDepth[coord_north_edge];
	float water_depth_east_edge =	tWaterDepth[coord_east_edge];
	float water_depth_south_edge =	tWaterDepth[coord_south_edge];
	float water_depth_west_edge =	tWaterDepth[coord_west_edge];

	// if the height difference this causes is larger then the depth, scale the flux accordingly
	float delta_height1 = time_step * volume_scale * (flux_north_edge.x + flux_north_edge.y + flux_north_edge.z + flux_north_edge.w);
	if (delta_height1 > water_depth_north_edge)
		flux_north_edge *= water_depth_north_edge / delta_height1;

	float delta_height2 = time_step * volume_scale * (flux_east_edge.x + flux_east_edge.y + flux_east_edge.z + flux_east_edge.w);
	if (delta_height2 > water_depth_east_edge)
		flux_east_edge *= water_depth_east_edge / delta_height2;

	float delta_height3 = time_step * volume_scale * (flux_south_edge.x + flux_south_edge.y + flux_south_edge.z + flux_south_edge.w);
	if (delta_height3 > water_depth_south_edge)
		flux_south_edge *= water_depth_south_edge / delta_height3;

	float delta_height4 = time_step * volume_scale * (flux_west_edge.x + flux_west_edge.y + flux_west_edge.z + flux_west_edge.w);
	if (delta_height4 > water_depth_west_edge)
		flux_west_edge *= water_depth_west_edge / delta_height4;

	// friction based on the wave height per water depth
	float fric1 = friction * water_depth_north_edge / (water_depth_north_edge + delta_height1);
	flux_north_edge *= pow(abs(fric1), time_step);

	float fric2 = friction * water_depth_east_edge / (water_depth_east_edge + delta_height2);
	flux_east_edge *= pow(abs(fric2), time_step);

	float fric3 = friction * water_depth_south_edge / (water_depth_south_edge + delta_height3);
	flux_south_edge *= pow(abs(fric3), time_step);

	float fric4 = friction * water_depth_west_edge / (water_depth_west_edge + delta_height4);
	flux_west_edge *= pow(abs(fric4), time_step);

	SetFlux(coord_north_edge, flux_north_edge);
	SetFlux(coord_east_edge, flux_east_edge);
	SetFlux(coord_south_edge, flux_south_edge);
	SetFlux(coord_west_edge, flux_west_edge);
}
