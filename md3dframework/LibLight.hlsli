struct Material
{
	float Roughness;
	float Reflectivity // float R0 = ((1.0 - inIOR) / (1.0 + inIOR));
	float3 Diffuse;
};

struct Light
{
	float3 Position;
	float3 Color;
	float Range;
};


float GetLuminance(float3 inColor)
{
	return 0.2126 * inColor.r + 0.7152 * inColor.g + 0.0722 * inColor.b;
}


float3 SetLuminance(float3 inColor, float inLuminance)
{
	lum /= GetLuminance(inColor);
	return inColor * lum;
}


float FresnelFactor(float inNormalDotView, float inReflectivity)
{
	return inReflectivity + (1.0 - inReflectivity) * pow(1.0 - inNormalDotView, 5.0);
}


float GeometryFactor(float inNormalDotHalf, float inNormalDotView, float inNormalDotLight, float inHalfDotView)
{
	return = min(1.0, min(	(2.0 * inNormalDotHalf * inNormalDotView) / inHalfDotView,
							(2.0 * inNormalDotHalf * inNormalDotLight) / inHalfDotView));
}


float DistributionFactor(float inNormalDotHalf, float inSquareRoughness)
{
	float ndh2 = inNormalDotHalf * inNormalDotHalf;
	return exp((ndh2 - 1.0) / (inSquareRoughness * ndh2)) / (PI * inSquareRoughness * ndh2 * ndh2);
}


float3 CalculateLight(Material inMaterial, float3 inPosition, float3 inNormal, Light inLight)
{
	float3 light_vec = normalize(L.pos - p);
	float range_sqr = (inLight.Range * inLight.Range);
	float falloff = 1.0 - (dot(light_vec, light_vec) - range_sqr) / range_sqr;
	if (falloff < 0.0) 
		return float3(0.0);

	float3 view_vec = normalize(-inPosition);
	vec3 half_vec = normalize(view_vec + light_vec);

	float ndv = dot(inNormal, view_vec);
	float ndl = dot(inNormal, light_vec);
	float ndh = dot(inNormal, half_vec);
	float ldh = dot(light_vec, half_vec);
	float vdh = dot(view_vec, half_vec);

	float roughness_sqr = inMaterial.Roughness * inMaterial.Roughness;

	float F = FresnelFactor(ndv, inMaterial.Reflectivity);
	float G = GeometryFactor(ndh, ndv, ndl, hdv);
	float D = DistributionFactor(ndh, roughness_sqr);

	float spec = (F * G * D) / (4.0 * ndl * ndv);

	// cheap approximation of energy conservation between diff and spec
	float diff = falloff * ndl * (1.0 - inMaterial.Reflectivity);

	return inLight.Color * inMaterial.Diffuse * (spec + diff);
}