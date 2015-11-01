RWTexture2D<float4> NW : register(u0);
RWTexture2D<float4> NE : register(u1);
RWTexture2D<float4> SW : register(u2);
RWTexture2D<float4> SE : register(u3);

cbuffer cPaintData
{
	float4	cPaintRect;
	float2	cPaintCoord;
	float2	cPaintRadius;
	int2	cResolution;
};

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord_in_brush = float2(DTid.x, DTid.y);
	float2 pixel_coord = cPaintRect.xy + coord_in_brush;
	float falloff = length((cPaintCoord - pixel_coord) / cPaintRadius);

	if (pixel_coord.y > cResolution.y)
	{
		if (pixel_coord.x > cResolution.x)
			NE[coord - cResolution] += falloff;
		else
			NW[coord - int2(0, cResolution.y)] += falloff;
	}
	else
	{
		if (pixel_coord.x > cResolution.x)
			SE[coord - int2(cResolution.x, 0)] += falloff;
		else
			SW[coord] += falloff;
	}
}