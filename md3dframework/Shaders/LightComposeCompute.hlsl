#include "LibHelper.hlsli"

RWTexture2D<float4> dst : register(u0);
Texture2D<float4> DirectDiffuse : register(t0);
Texture2D<float4> DirectSpecular : register(t1);
Texture2D<float4> Indirect : register(t2);
Texture2D<float4> Reflections : register(t3);
Texture2D<float> Depth : register(t4);
Texture2D<float> HalfDepth : register(t5);


SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	// coord from bottom left of brush rect
	float2 coord = float2(DTid.x, DTid.y);
	float2 tex_size;
	dst.GetDimensions(tex_size.x, tex_size.y);
	float2 uv = float2(coord + 0.5) / (tex_size);
	float hi_depth = Depth[coord];
	float4 lo_depth = HalfDepth.GatherRed(LinearSampler, uv);
	lo_depth.x = HalfDepth[(coord - 1) / 2];
	lo_depth.y = HalfDepth[(coord - 1) / 2 + uint2(1, 0)];
	lo_depth.z = HalfDepth[(coord - 1) / 2 + uint2(0, 1)];
	lo_depth.w = HalfDepth[(coord - 1) / 2 + uint2(1, 1)];

	bool samp_bilinear;
	uint2 half_coord = NearestDepthSelect(coord, lo_depth, hi_depth, samp_bilinear);

	float4 indirect;
	indirect = (samp_bilinear == true) ? Indirect.SampleLevel(LinearSampler, uv, 0) : Indirect[half_coord];

	dst[coord] = indirect;// +DirectDiffuse[coord] + DirectSpecular[coord];// +Reflections[coord];
}
