#include "LibSimplexNoiseShader.hlsli"

RWTexture2D<float> rwTarget : register(u0);


cbuffer cPaintData
{
	int4	cPaintRect;		// top left bottom right
	float4	cPaintData;		// height add, falloff radius fraction, world_pixel_position.xy
};


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int2 coord_in_brush = DTid.xy;

	int2 target_pixel = cPaintRect.xy + coord_in_brush;
	int2 world_pixel_position = cPaintData.zw;

	float2 radius = (cPaintRect.zw - cPaintRect.xy) / 2.0;
	float2 brush_center_pixel = cPaintRect.xy + radius;

	float height_add = cPaintData.x;
	float falloff_start_frac = cPaintData.y;

	float falloff = length((float2(target_pixel) - brush_center_pixel) / radius);
	falloff = saturate((1.0 - falloff) / falloff_start_frac);
	falloff = smoothstep(0.0, 1.0, falloff);

	rwTarget[target_pixel] += falloff * height_add * snoise(0.1 * float2(world_pixel_position + coord_in_brush - radius));
}
