RWTexture2D<float4> rwNormal :		register(u0);

Texture2D<float> tHeightCenter :	register(t0);
Texture2D<float> tHeightNorth :		register(t1);
Texture2D<float> tHeightEast :		register(t2);
Texture2D<float> tHeightSouth :		register(t3);
Texture2D<float> tHeightWest :		register(t4);


cbuffer cConstantData : register(b0)
{
	float4	cTerrainScale;
	int4	cTextureInfo;
}

// TODO optimize so this only happens for selective border pixels
float SampleHeight(int2 inCoord)
{
	int tex_index = 0;
	int tex_width = cTextureInfo.x;
	int tex_height = cTextureInfo.y;

	if (inCoord.x < 0)
	{
		inCoord.x = tex_width - 2;
		return tHeightWest[inCoord];
	}
	else if (inCoord.x >= tex_width)
	{
		inCoord.x = 1;
		return tHeightEast[inCoord];
	}
	if (inCoord.y < 0)
	{
		inCoord.y = tex_height - 2;
		return tHeightSouth[inCoord];
	}
	else if (inCoord.y >= tex_height)
	{
		inCoord.y = 1;
		return tHeightNorth[inCoord];
	}
	else
	{
		return tHeightCenter[inCoord];
	}
}



[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 coord = DTid.xy;

	if ((coord.x < 0) || (coord.y < 0) || (coord.x >= cTextureInfo.x) || (coord.y >= cTextureInfo.y))
		return;

	float north = SampleHeight(coord + int2(0, 1));
	float east = SampleHeight(coord + int2(1, 0));
	float south = SampleHeight(coord - int2(0, 1));
	float west = SampleHeight(coord - int2(1, 0));

	float2 meter_step = cTerrainScale.xy / cTextureInfo.xy;
	float3 dx = float3(2.0 * meter_step.x,	0.0,				cTerrainScale.z * (east - west));
	float3 dy = float3(0.0,					2.0 * meter_step.y,	cTerrainScale.z * (north - south));
	rwNormal[coord] = float4(-normalize(cross(dy, dx)) * 0.5 + 0.5, 1.0);
}
