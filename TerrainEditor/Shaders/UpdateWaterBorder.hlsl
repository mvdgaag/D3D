RWTexture2D<float> rwWaterDepth :	register(u0);
RWTexture2D<float> rwWaterHeight :	register(u1);
Texture2D<float> tTerrain :			register(t0);

Texture2D<float> tFlux :			register(t1);
Texture2D<float> tFluxNorth :		register(t2);
Texture2D<float> tFluxEast :		register(t3);
Texture2D<float> tFluxSouth :		register(t4);
Texture2D<float> tFluxWest :		register(t5);


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

	int select = DTid.y;

	int2 coord;
	if (select == 0)		coord = int2(DTid.x, tex_size.y - 1);
	else if (select == 1)	coord = int2(tex_size.x - 1, DTid.x);
	else if (select == 2)	coord = int2(DTid.x, 0);
	else					coord = int2(0, DTid.x);

	// gather parameters
	const float height_scale =	cParams.x;
	const float pixel_area =	cParams.y;
	const float time_step =		cParams.z;
	const float volume_scale =	cParams.w;
	
	// the water depth and terrain height
	float water_depth =		rwWaterDepth[coord];
	float terrain_height =	tTerrain[coord];

	// the flux in and out of the texels
	float4 out_flux = GetFlux(tFlux, coord);
	float4 in_flux;
	in_flux.x = (coord.y == tex_size.y - 1) ?	GetFlux(tFluxNorth, int2(coord.x, 0)).z :				GetFlux(tFlux, coord + int2(0, 1)).z;
	in_flux.y = (coord.x == tex_size.x - 1) ?	GetFlux(tFluxEast, int2(0, coord.y)).w :				GetFlux(tFlux, coord + int2(1, 0)).w;
	in_flux.z = (coord.y == 0) ?				GetFlux(tFluxSouth, int2(coord.x, tex_size.y - 1)).x :	GetFlux(tFlux, coord - int2(0, 1)).x;
	in_flux.w = (coord.x == 0) ?				GetFlux(tFluxWest, int2(tex_size.x - 1, coord.y)).y	:	GetFlux(tFlux, coord - int2(1, 0)).y;

	// the sum of all incoming and outgoing flux
	float sum_flux =	(in_flux.x + in_flux.y + in_flux.z + in_flux.w) - 
						(out_flux.x + out_flux.y + out_flux.z + out_flux.w);

	// update the water depth
	water_depth += time_step * volume_scale * sum_flux;
	water_depth += 0.001; // rain

	// cannot be negative
	water_depth = max(0.0, water_depth);
	
	// set north east borders
	rwWaterDepth[coord] = water_depth;
	rwWaterHeight[coord] = water_depth + terrain_height;
}
