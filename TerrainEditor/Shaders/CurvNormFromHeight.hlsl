RWTexture2D<float> rwNormal :		register(u0);
RWTexture2D<float> rwCurvature :	register(u1);

Texture2D<float> tHeightCenter :	register(t0);
Texture2D<float> tHeightNorth :		register(t1);
Texture2D<float> tHeightEast :		register(t2);
Texture2D<float> tHeightSouth :		register(t3);
Texture2D<float> tHeightWest :		register(t4);


cbuffer cConstantData : register(b0)
{
	float4 cTerrainScale;
}


float SampleHeight(int2 inCoord, int2 inTexSize)
{
	int3 tex_size = int3(inTexSize, 0);

	if (inCoord.x < 0)
		return tHeightWest[inCoord + tex_size.xz];
	else if (inCoord.x >= tex_size.x)
		return tHeightEast[inCoord - tex_size.xz];
	else if (inCoord.y < 0)
		return tHeightSouth[inCoord + tex_size.zy];
	else if (inCoord.y >= tex_size.y)
		return tHeightNorth[inCoord - tex_size.zy];
	else
		return tHeightCenter[inCoord];
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 coord = DTid.xy;
	int2 tex_size;
	tHeightCenter.GetDimensions(tex_size.x, tex_size.y);

	if ((coord.x < 0) || (coord.y < 0) || (coord.x >= tex_size.x) || (coord.y >= tex_size.y))
		return;

	float north = SampleHeight(coord + int2(0, 1), tex_size);
	float east = SampleEast(coord + int2(1, 0), tex_size);
	float south = SampleSouth(coord - int2(0, 1), tex_size);
	float west = SampleWest(coord - int2(1, 0), tex_size);

	float2 rcp_tex_size = 1.0 / tex_size;

	float3 dx = float3(2.0 * rcp_tex_size.x, (cTerrainScale.z / cTerrainScale.x) * (east - west), 0.0);
	float3 dz = float3(0.0, (cTerrainScale.z / cTerrainScale.y) * (north - south), 2.0 * rcp_tex_size.y);
	rwNormal[coord] = normalize(cross(dz, dx));

	// todo: curvature
}
