#include "LibSimplexNoiseShader.hlsli"

RWTexture2D<float4>		rwTarget :	register(u0);
Texture2D<float4>		tBase :		register(t0);


cbuffer cPaintData
{
	int4	cPaintRect;		// top left bottom right
	int4	cTextureInfo;	// width height 0 0
	float4	cPaintData;		// worldPosition.xy
	float4	cBrushData;		// radius, height add, falloff radius fraction
	float4	cColor;			// RGBA
	float4	cTileWorldRect; // top left bottom right
};


float2 TextureToWorld(int2 inTexel, int2 inTexSize, float4 inTileWorldRect)
{
	// first texel uv = 0.0, last texel uv = 1.0, so that texels overlap with verts and verts on tile borders touch
	float2 uv = float2(inTexel) / float2(inTexSize - int2(1, 1));
		return inTileWorldRect.xy + uv * (inTileWorldRect.zw - inTileWorldRect.xy);
}


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 coord_in_brush = DTid.xy;
	int2 target_pixel = cPaintRect.xy + coord_in_brush;

	if (target_pixel.x < 0 || target_pixel.y < 0 || target_pixel.x >= cTextureInfo.x || target_pixel.y >= cTextureInfo.y)
		return;

	float2 world_pos = TextureToWorld(target_pixel, cTextureInfo.xy, cTileWorldRect);
	float2 world_brush_center = cPaintData.xy;
	float world_radius = cBrushData.x;
	float height_add = cBrushData.y;
	float falloff_start_frac = cBrushData.z;

	float falloff = length((world_pos - world_brush_center) / world_radius);
	falloff = saturate((1.0 - falloff) / falloff_start_frac);
	falloff = smoothstep(0.0, 1.0, falloff);

	float noise = 0.25 * snoise(0.1 * world_pos);
	noise += 0.5 * snoise(0.05 * world_pos + 42.0);
	noise += snoise(0.025 * world_pos + 666.0);
	noise /= 1.75;

	rwTarget[target_pixel] = tBase[target_pixel] + cColor * falloff * height_add * noise;
}
