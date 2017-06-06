RWTexture2D<float> rwTarget : register(u0);
Texture2D<float> neighbor_hor :  register(t0);
Texture2D<float> neighbor_ver :  register(t1);
Texture2D<float> neighbor_dia :  register(t2);

cbuffer cPaintData
{
	int4	cPaintRect;		// top left bottom right
	int4	cTextureInfo;	// width height 0 0
	float4	cPaintData;		// height add, falloff radius fraction, worldPosition.xy
};


float SampleHeight(int2 inCoord)
{
	int tex_index = 0;
	int tex_width = cTextureInfo.x;
	int tex_height = cTextureInfo.y;

	if (inCoord.x < 0)
	{
		inCoord.x += tex_width - 1; // minus one, because the border pixels overlap
		tex_index = 1;
	}
	else if (inCoord.x >= tex_width) 
	{
		inCoord.x -= tex_width - 1; // minus one, because the border pixels overlap
		tex_index = 1;
	}

	if (inCoord.y < 0)
	{
		inCoord.y += tex_height - 1; // minus one, because the border pixels overlap
		tex_index += 2;
	}
	else if (inCoord.y >= tex_height)
	{
		inCoord.y -= tex_height - 1; // minus one, because the border pixels overlap
		tex_index += 2;
	}

	if (tex_index == 0)
	{
		return rwTarget[inCoord];
	}
	else if (tex_index == 1)
	{
		return neighbor_hor[inCoord];
	}
	else if (tex_index == 2)
	{
		return neighbor_ver[inCoord];
	}
	else
	{
		return neighbor_dia[inCoord];
	}
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 coord_in_brush = DTid.xy;
	int2 target_pixel = cPaintRect.xy + coord_in_brush;
	
	if (target_pixel.x < 0 || target_pixel.x >= cTextureInfo.x || target_pixel.y < 0 || target_pixel.y >= cTextureInfo.y)
		return;

	float2 radius = (cPaintRect.zw - cPaintRect.xy) / 2.0;
	float2 brush_center_pixel = cPaintRect.xy + radius;

	float falloff_start_frac = cPaintData.y;
	float falloff = length((float2(target_pixel) - brush_center_pixel) / radius);
	falloff = saturate((1.0 - falloff) / falloff_start_frac);
	falloff = smoothstep(0.0, 1.0, falloff);

	float original = rwTarget[target_pixel];
	float neighbors = 0.0;
	neighbors += SampleHeight(target_pixel + int2( 1, 0));
	neighbors += SampleHeight(target_pixel + int2( 0, 1));
	neighbors += SampleHeight(target_pixel + int2(-1, 0));
	neighbors += SampleHeight(target_pixel + int2( 0,-1));

	rwTarget[target_pixel] = lerp(original, (original + neighbors) / 5.0, falloff);
}
