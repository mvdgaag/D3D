#include "LibHelper.hlsli"
#include "LibLight.hlsli"


RWTexture2D<float4> dst : register(u0);
Texture2D<float> LinearDepth : register(t0);
Texture2D<half2> Normal : register(t1);
Texture2D<float4> Diffuse : register(t2);
Texture2D<float4> Material : register(t3);


cbuffer cLightingShaderConstants : register(b0)
{
	float2	cViewReconstructionVector; // tan(fovx)/near, tan(fovy)/near
	float2	cTargetSize;
	float4	cFrameData;
};


SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};


[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 coord = float2(DTid.x, DTid.y);
	float linear_depth = LinearDepth[coord];
	half3 normal = DecodeNormal(Normal[coord].xy);
	half3 diffuse = Diffuse[coord].xyz;
	half4 material = Material[coord];

	myMaterial m;
	m.Roughness = 0.4;
	m.Reflectivity = 0.4;
	m.Diffuse = diffuse;

	myLight l;
	float a = cFrameData.x / 100.0;
	l.Position = float3(1.0, 2.0, 1.0) * 10.0;
	l.Color = float3(0.9, 0.9, 0.9);
	l.Range = 30.0;

	float3 p = ReconstructCSPosition(coord / cTargetSize, linear_depth, cViewReconstructionVector);

	float3 val = CalculateLight(m, p, normal, l);
	dst[coord] = float4(val, 1);
}
