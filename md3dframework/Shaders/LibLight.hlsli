
struct Material
{
	float Roughness;
	float Reflectivity; // float R0 = ((1.0 - inIOR) / (1.0 + inIOR));
	float3 Diffuse;
};


struct PointLight
{
	float3 Position;
	float3 Color;
	float Range;
};


// Schlick
float FresnelFactor(float inNdV, float inReflectivity)
{
	return inReflectivity + (1.0 - inReflectivity) * pow(1.0 - inNdV, 5.0);
}

// Smith matching Walter GGX
float GeometryFactor(float inNdV, float inNdL, float inAlpha2)
{
	float GV = inNdV + sqrt(inAlpha2 + (1.0 - inAlpha2) * inNdV * inNdV);
	float GL = inNdL + sqrt(inAlpha2 + (1.0 - inAlpha2) * inNdL * inNdL);
	return 1.0 / (GV * GL);
}

// Walter GGX
float DistributionFactor(float inNdH, float inAlpha2)
{
	return inAlpha2 / (3.1415 * pow(inNdH * inNdH * (inAlpha2 - 1.0) + 1.0, 2));
}


float OrenNayarDiffuse(float inNdL, float inNdV, float3 inNormal, float inViewVec, float inAlpha)
{
	if (inNdL <= 0)
		return 0;

	float a = 1.0 - 0.5 * (inAlpha / (inAlpha + 0.57));
	float b = 0.45 * (inAlpha / (inAlpha + 0.09));

	float ga = dot(inViewVec - inNormal * inNdV, inNormal - inNormal * inNdL);

	return inNdL * (a + b * max(0.0, ga) * 
		sqrt((1.0 - inNdV * inNdV) * (1.0 - inNdL * inNdL)) / max(inNdL, inNdV));
}


void AccumulateLight(Material inMaterial, float3 inPosition, float3 inNormal, PointLight inLight, inout float3 ioDiffuseAccum, inout float3 ioSpecularAccum)
{
	float3 light_vec = inLight.Position - inPosition;
	float range_sqr = (inLight.Range * inLight.Range);
	float falloff = saturate((range_sqr - dot(light_vec, light_vec)) / range_sqr);
	if (falloff <= 0.0)
		return;

	light_vec = normalize(light_vec);
	float3 normal = inNormal;
	float3 view_vec = normalize(-inPosition);
	float3 half_vec = normalize(view_vec + light_vec);

	float ndl = saturate(dot(normal, light_vec));
	float ndv = saturate(dot(normal, view_vec));
	float ndh = saturate(dot(normal, half_vec));
	float ldh = saturate(dot(light_vec, half_vec));
	float vdh = saturate(dot(view_vec, half_vec));

	float alpha = inMaterial.Roughness * inMaterial.Roughness;
	float alpha2 = alpha * alpha;

	float F = FresnelFactor(ndv, inMaterial.Reflectivity);
	float G = GeometryFactor(ndv, ndl, alpha2);
	float D = DistributionFactor(ndh, alpha2);

	float spec = F * G * D;

	float3 diff = inMaterial.Diffuse * falloff * OrenNayarDiffuse(ndl, ndv, normal, view_vec, alpha);
	ioSpecularAccum += inLight.Color * spec;
	ioDiffuseAccum += inLight.Color * diff * (1.0 - inMaterial.Reflectivity);// / 3.1415;
}
