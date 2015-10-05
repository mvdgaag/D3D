#include "LibNormals.hlsli"
#include "LibLight.hlsli"

RWTexture2D<float4> dst : register(u0);
Texture2D<float> LinearDepth : register(t0);
Texture2D<half2> Normal : register(t1);
Texture2D<float4> Diffuse : register(t2);
Texture2D<float4> Material : register(t3);


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
	float linear_depth = LinearDepth[coord] / 5.0;
	half3 normal = AzimuthalDecodeNormal(Normal[coord].xy);
	half3 diffuse = Diffuse[coord].xyz;
	half4 material = Material[coord];

	myMaterial m;
	m.Roughness = material.x;
	m.Reflectivity = material.y;
	m.Diffuse = diffuse;

	myLight l;
	l.Position = float3(10.0, -10.0, -10.0);
	l.Color = float3(0.6, 0.5, 0.4);
	l.Range = 30.0;

	float3 p = float3(0, 0, 1);

	float3 val = CalculateLight(m, p, normal, l);
	
	dst[coord] = float4(val,1);
}
