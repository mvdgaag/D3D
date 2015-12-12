
struct Material
{
	float	mRoughness;
	float	mReflectivity; // float R0 = ((1.0 - inIOR) / (1.0 + inIOR));
	float3	mDiffuse;
};


struct Light
{
	float3	mDirection;
	float3	mColor;
	float	mAttenuation;
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


void AccumulateLight(Material inMaterial, float3 inPosition, float3 inNormal, Light inLight, inout float3 ioDiffuse, inout float3 ioSpecular)
{
	float3 normal = inNormal;
	float3 view_vec = normalize(-inPosition);
	float3 half_vec = normalize(view_vec + inLight.mDirection);

	float ndl = saturate(dot(normal, inLight.mDirection));
	float ndv = saturate(dot(normal, view_vec));
	float ndh = saturate(dot(normal, half_vec));

	float alpha = inMaterial.mRoughness * inMaterial.mRoughness;
	float alpha2 = alpha * alpha;

	float3 diff = inMaterial.mDiffuse * (1.0 - inMaterial.mReflectivity) * OrenNayarDiffuse(ndl, ndv, normal, view_vec, alpha);

	float F = FresnelFactor(ndv, inMaterial.mReflectivity);
	float G = GeometryFactor(ndv, ndl, alpha2);
	float D = DistributionFactor(ndh, alpha2);

	float spec = ndl * F * G * D;

	ioSpecular += inLight.mColor * inLight.mAttenuation * spec;
	ioDiffuse += inLight.mColor * inLight.mAttenuation * diff;
}
