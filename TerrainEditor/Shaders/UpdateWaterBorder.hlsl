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

	int2 coord_north_edge = int2(DTid.x, tex_size.y - 1);
	int2 coord_east_edge =	int2(tex_size.x - 1, DTid.x);

	// gather parameters
	const float height_scale =	cParams.x;
	const float pixel_area =	cParams.y;
	const float time_step =		cParams.z;
	const float volume_scale =	cParams.w;
	
	// the water depth and terrain height
	float water_depth_north_edge =		rwWaterDepth[coord_north_edge];
	float terrain_height_north_edge =	tTerrain[coord_north_edge];
	float water_depth_east_edge =		rwWaterDepth[coord_east_edge];
	float terrain_height_east_edge =	tTerrain[coord_east_edge];

	// the flux in and out of the texels
	float4 out_flux_north_edge = GetFlux(tFlux, coord_north_edge);
	float4 in_flux_north_edge;
	in_flux_north_edge.x = GetFlux(tFluxNorth, int2(DTid.x, 0)).z;			// from north, to south
	in_flux_north_edge.y = GetFlux(tFlux, coord_north_edge + int2(1, 0)).w; // from east, to west
	in_flux_north_edge.z = GetFlux(tFlux, coord_north_edge - int2(0, 1)).x; // from south, to north
	in_flux_north_edge.w = GetFlux(tFlux, coord_north_edge - int2(1, 0)).y;	// from west, to east
	float4 out_flux_east_edge = GetFlux(tFlux, coord_east_edge);
	float4 in_flux_east_edge;
	in_flux_east_edge.x = GetFlux(tFlux, coord_east_edge + int2(0, 1)).z;	// from north, to south
	in_flux_east_edge.y = GetFlux(tFluxEast, int2(0, DTid.x)).w;			// from east, to west
	in_flux_east_edge.z = GetFlux(tFlux, coord_east_edge - int2(0, 1)).x;	// from south, to north
	in_flux_east_edge.w = GetFlux(tFlux, coord_east_edge - int2(1, 0)).y;	// from west, to east

	// the sum of all incoming and outgoing flux
	float sum_flux_north_edge = (in_flux_north_edge.x + in_flux_north_edge.y + in_flux_north_edge.z + in_flux_north_edge.w) - 
								(out_flux_north_edge.x + out_flux_north_edge.y + out_flux_north_edge.z + out_flux_north_edge.w);
	float sum_flux_east_edge =	(in_flux_east_edge.x + in_flux_east_edge.y + in_flux_east_edge.z + in_flux_east_edge.w) - 
								(out_flux_east_edge.x + out_flux_east_edge.y + out_flux_east_edge.z + out_flux_east_edge.w);

	// update the water depth
	water_depth_north_edge += time_step * volume_scale * sum_flux_north_edge;
	water_depth_north_edge += 0.001; // rain
	water_depth_east_edge += time_step * volume_scale * sum_flux_east_edge;
	water_depth_east_edge += 0.001; // rain

	// cannot be negative
	water_depth_north_edge =	max(0.0, water_depth_north_edge);
	water_depth_east_edge =		max(0.0, water_depth_east_edge);
	
	// set north east borders
	rwWaterDepth[coord_north_edge] =	water_depth_north_edge;
	rwWaterHeight[coord_north_edge] =	water_depth_north_edge + terrain_height_north_edge;
	rwWaterDepth[coord_east_edge] =		water_depth_east_edge;
	rwWaterHeight[coord_east_edge] =	water_depth_east_edge + terrain_height_east_edge;

	// copy south west borders
	// doesn't do a thing?
	int2 coord_south_edge =	int2(DTid.x, 0);
	int2 coord_west_edge =	int2(0, DTid.x);
	rwWaterDepth[coord_south_edge] =	tWaterDepthSouth[coord_north_edge];
	rwWaterHeight[coord_south_edge] =	tWaterHeightSouth[coord_north_edge];
	rwWaterDepth[coord_west_edge] =		tWaterDepthWest[coord_east_edge];
	rwWaterHeight[coord_west_edge] =	tWaterHeightWest[coord_east_edge];
}
