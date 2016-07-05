#ifdef LIB_LIGHT
#else
#define LIB_LIGHT

#include "LibHelper.hlsli"

#define PI 3.14159265359
#define HALF_PI 1.57079632679

struct Material
{
	float	metalicity;
	float	roughness;
	float	reflectance; // float R0 = ((1.0 - inIOR) / (1.0 + inIOR));
	float3	diffuse;
};


struct Light
{
	float3	direction;
	float3	color;
	float	attenuation;
	bool	hasShadowMap;
};



/* 
**
** Imprtance sampling functions  
**
*/

float radicalInverse_VdC(uint bits) 
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}


float2 Hammersley(uint i, uint N) 
{
	return float2(float(i) / max(0.0001, float(N)), radicalInverse_VdC(i));
}


// TODO: alpha*alpha or alpha in CosTheta
// alpha*alpha seems to best match analytical light falloff, but needs proof
float3 ImportanceSampleGGX(float2 Xi, float alpha, float3 N)
{
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (alpha*alpha - 1) * Xi.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);

	float3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);

	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}


float3 ImportanceSampleCosine(float2 Xi, float3 N)
{
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt(1 - Xi.y);
	float SinTheta = sqrt(1 - CosTheta * CosTheta);

	float3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);

	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}


/*
**
** Diffuse functions
**
*/


float OrenNayar(float dotNL, float dotNV, float3 N, float V, float alpha)
{
	if (dotNL <= 0)
		return 0;

	float a = 1.0 - 0.5 * (alpha / (alpha + 0.57));
	float b = 0.45 * (alpha / (alpha + 0.09));
	float ga = dot(V - N * dotNV, N - N * dotNL);
	return dotNL * (a + b * max(0.0, ga) * sqrt((1.0 - dotNV * dotNV) * (1.0 - dotNL * dotNL)) / max(dotNL, dotNV));
}


/*
**
** Fresnel functions
**
*/


float3 F_GGX(float dotLX, float3 F0, float3 F90)
{
	return F0 + (F90 - F0) * pow(1.0 - dotLX, 5.0);
}


/*
**
** Shadowing/visibility functions
**
*/

/*
float Unoptimized_G_GGX_Correlated(float dotNL, float dotNV, float alpha)
{
	float alpha2 = alpha * alpha;
	float dotNL2 = dotNL * dotNL;
	float dotNV2 = dotNV * dotNV;

	float lambda_v = (-1.0 + sqrt(1.0 + alpha2 * (1.0 - dotNV2) / dotNV2)) / 2.0;
	float lambda_l = (-1.0 + sqrt(1.0 + alpha2 * (1.0 - dotNL2) / dotNL2)) / 2.0;
	return (dotNL * dotNV) / (1.0 + lambda_v + lambda_l );
}


float Unoptimized_V_GGXCorrelated(float dotNL, float dotNV, float alpha)
{
	return Unoptimized_G_GGX_Correlated(dotNL, dotNV, alpha) / (4.0 * dotNL * dotNV);
}
*/

float G_GGX_Correlated(float dotNL, float dotNV, float alpha)
{
	float alpha2 = alpha * alpha;
	float dotNL2 = dotNL * dotNL;
	float dotNV2 = dotNV * dotNV;

	float tmp = (1.0 / alpha2) - 1.0;
	return (2.0 * dotNL * dotNV) / (alpha * (sqrt(tmp + 1.0 / dotNL2) + sqrt(tmp + 1.0 / dotNV2)));
}


float V_GGXCorrelated(float dotNL, float dotNV, float alpha)
{
	float alpha2 = alpha * alpha;
	float dotNL2 = dotNL * dotNL;
	float dotNV2 = dotNV * dotNV;

	float tmp = (1.0 / alpha2) - 1.0;
	return 0.5 / (alpha * (sqrt(tmp + 1.0 / dotNL2) + sqrt(tmp + 1.0 / dotNV2)));
}


/*
**
** Microfacet Distribution functions
**
*/


// GGX Trowbridge-Reitz
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
float D_GGX(float dotNH, float alpha)
{
	float tmp = alpha / max(1e-8, (dotNH * dotNH * (alpha * alpha - 1.0) + 1.0));
	return tmp * tmp / PI;
}


/*
	dotNH = dot(l+v,n) / |l+v|
*/

/*
**
** Brute-force reference functions
**
*/


float3 BruteForceSpecularIBL(TextureCube EnvMap, SamplerState EnvMapSampler, float3 SpecularColor, float m, float3 N, float3 V)
{
	float3 SpecularLighting = 0;
	const uint kNumSamples = 512;
	float alpha = m*m;

	for (uint i = 0; i < kNumSamples; i++)
	{
		float2 Xi = Hammersley(i, kNumSamples);
		float3 H = ImportanceSampleGGX(Xi, alpha, N);
		float3 L = 2 * dot(V, H) * H - V;

		float dotNL = saturate(dot(N, L));
		if (dotNL > 0)
		{
			// weight = fr * dotNL
			// with : fr = D(H) * F(H) * G(V, L) / ( 4 * dotNL * dotNV )
			//
			// Since we integrate in the microfacet space, we include the Jacobian of the transform
			// pdf = D(H) * dotNH / ( 4 * dotLH )
			//
			// weight / pdf = ( dotNL * D(H) * F(H) * G(V, L) * 4 * dotLH ) / ( 4 * dotNL * dotNV * D(H) * dotNH ) =
			// F(H) * G(V, L) * dotLH / DotNV * dotNH;

			float dotNV = saturate(dot(N, V));
			float dotNH = saturate(dot(N, H));
			float dotLH = saturate(dot(L, H));

			float3 F = F_GGX(dotLH, SpecularColor, float3(1.0, 1.0, 1.0));
			float G = G_GGX_Correlated(dotNL, dotNV, alpha);
			
			SpecularLighting += EnvMap.SampleLevel(EnvMapSampler, L, 0).rgb * F * G * dotLH / (dotNV * dotNH);
		}
	}
	return SpecularLighting / kNumSamples;
}


float3 BruteForceDiffuseIBL(TextureCube EnvMap, SamplerState EnvMapSampler, float3 SpecularColor, float m, float3 N, float3 V)
{
	float3 DiffuseLighting = 0;
	const uint kNumSamples = 512;

	// TODO: take into account roughness
	// energy conservation with specular
	for (uint i = 0; i < kNumSamples; i++)
	{
		float2 Xi = Hammersley(i, kNumSamples);
		float3 L = ImportanceSampleCosine(Xi, N);
		DiffuseLighting += EnvMap.SampleLevel(EnvMapSampler, L, 0).rgb;
	}
	return DiffuseLighting / kNumSamples;
}


/*
**
** Integration/prebaking functions
**
*/


float2 IntegrateBRDF(float alpha, float dotNV)
{
	float3 V;
	V.x = sqrt(1.0f - dotNV * dotNV); // sin
	V.y = 0;
	V.z = dotNV; // cos

	float A = 0;
	float B = 0;
	
	const uint kNumSamples = 1024;
	for (uint i = 0; i < kNumSamples; i++)
	{
		float2 Xi = Hammersley(i, kNumSamples);
		float3 H = ImportanceSampleGGX(Xi, alpha, float3(0.0, 0.0, 1.0));
		float3 L = 2 * dot(V, H) * H - V;

		float dotNL = saturate(L.z);
		float dotNH = saturate(H.z);
		float dotVH = saturate(dot(V, H));
	
		if (dotNL > 0)
		{
			float G = G_GGX_Correlated(dotNL, dotNV, alpha);
			float vis = G * dotVH / (dotNH * dotNV);
			float Fc = pow(1 - dotVH, 5);
			A += (1 - Fc) * vis;
			B += Fc * vis;
		}
	}

	return float2(A, B) / kNumSamples;
}


float3 PrefilterEnvMap(TextureCube EnvMap, SamplerState EnvMapSampler, float alpha, float3 R)
{
	float3 N = R;
	float3 V = R;
	float3 prefiltered_color = 0;
	float  total_weight = 0;

	// TODO: higher numbers crash compute shader on timeout!
	const uint kNumSamples = 512;

	for (uint i = 0; i < kNumSamples; i++)
	{
		float2 Xi = Hammersley(i, kNumSamples);
		float3 H = ImportanceSampleGGX(Xi, alpha, N);
		float3 L = 2 * dot(V, H) * H - V;
		float dotNL = saturate(dot(N, L));

		if (dotNL > 0)
		{
			prefiltered_color += EnvMap.SampleLevel(EnvMapSampler, L, 0).rgb * dotNL;
			total_weight += dotNL;
		}
	}

	return prefiltered_color / total_weight;
}


// http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr_v2.pdf
float3 getSpecularDominantDir(float3 N, float3 R, float roughness)
{
	float smoothness = saturate(1 - roughness);
	float lerpFactor = smoothness * (sqrt(smoothness) + roughness);
	 // The result is not normalized as we fetch in a cubemap
	return lerp(N, R, lerpFactor);
}



float3 ApproximateSpecularIBL(TextureCube EnvMap, SamplerState EnvMapSampler, float EnvMapMaxMip, Texture2D BRDFLookupTexture, SamplerState BRDFLookupSampler, float3 SpecularColor, float m, float3 N, float3 V)
{
	float alpha = m * m;

	float dotNV = saturate(dot(N, V));
	float3 R = 2 * dot(V, N) * N - V;

	// HACK attempt to approximate sampling the dominant specular direction
	// m^3 seems to match brute force highlight locations better than m^2 and m^4
	//R = lerp(R, N, m*alpha);
	R = getSpecularDominantDir(N, R, m);

	float mip = m * (EnvMapMaxMip - 2);

	float3 prefiltered_color = EnvMap.SampleLevel(EnvMapSampler, R, mip);
	float2 env_brdf = BRDFLookupTexture.SampleLevel(BRDFLookupSampler, float2(alpha, dotNV), 0);

	// uncomment to skip lookups and generate from mip0
	//float3 prefiltered_color = PrefilterEnvMap(EnvMap, EnvMapSampler, alpha, R);
	//float2 env_brdf = IntegrateBRDF(alpha, dotNV);

	return prefiltered_color * (SpecularColor * env_brdf.x + env_brdf.y);
}


/*
**
** Lighting functions
**
*/


void BRDF_GGX(float3 N, float3 V, float3 L, float alpha, float3 F0, out float3 Spec, out float3 Diff)
{
	float3 H = normalize(V + L);

	float dotNL = saturate(dot(N, L));
	float dotNV = saturate(dot(N, V));
	float dotNH = saturate(dot(N, H));
	float dotLH = saturate(dot(L, H));

	if (dotNL > 0)
	{
		float D = D_GGX(dotNH, alpha);
		float F = F_GGX(dotLH, F0, float3(1.0, 1.0, 1.0));
		float vis = V_GGXCorrelated(dotNL, dotNV, alpha);

		Spec = D * F * vis; // = D * F * G / (4 * dotNL * dotNV)
		Diff = (1 - F) / PI;
	}
	else
	{
		Spec = Diff = 0;
	}
}


void AccumulateLight(Material inMaterial, float3 inPosition, float3 inNormal, Light inLight, inout float3 ioDiffuse, inout float3 ioSpecular)
{
	float3 N =			inNormal;
	float3 V =			normalize(-inPosition);
	float3 L =			inLight.direction;
	float m =			max(0.1, saturate(inMaterial.roughness));
	float alpha =		m * m;
	float F0 =			max(0.02, saturate(inMaterial.reflectance));

	float3 specular_color = lerp(float3(F0, F0, F0), (F0 * 2.0) * inMaterial.diffuse.rgb, inMaterial.metalicity);
	float3 diffuse_color = inMaterial.diffuse.rgb * (1 - inMaterial.metalicity);
	
	// TODO: this is double calculated in the brdf function!
	float dotNL = saturate(dot(N, L));

	float3 S, D;
	BRDF_GGX(N, V, L, alpha, specular_color, S, D);
	
	float irradiance = inLight.color * inLight.attenuation * dotNL;
	ioDiffuse += D * diffuse_color * irradiance;
	ioSpecular += S * specular_color * irradiance;
}


void AccumulateLightOpt(Material inMaterial, float3 inPosition, float3 inNormal, Light inLight, inout float3 ioDiffuse, inout float3 ioSpecular)
{
	float3 N = inNormal;
	float3 V = normalize(-inPosition);
	float3 L = inLight.direction;
	float m = max(0.1, saturate(inMaterial.roughness));
	float alpha = m * m;
	float F0 = max(0.02, saturate(inMaterial.reflectance));

	float3 specular_color = lerp(float3(F0, F0, F0), (F0 * 2.0) * inMaterial.diffuse.rgb, inMaterial.metalicity);
	float3 diffuse_color = inMaterial.diffuse.rgb * (1 - inMaterial.metalicity);

	float dotNL = saturate(dot(N, L));
	float dotNV = saturate(dot(N, V));	
	float dotVL = saturate(dot(V, L));

	float alpha2 = alpha * alpha;
	float dotNL2 = dotNL * dotNL;
	float dotNV2 = dotNV * dotNV;
	float dotNH2 = (dotNL + dotNV) * (dotNL + dotNV) / (2.0 + 2.0 * dotVL);

	// Denominator of GGX height correlated visibility term = 1/vis where vis = G/(4*dotNL*dotNV)
	float tmp = (1.0 / alpha2) - 1.0;
	float vis_denom = 2 * (alpha * (sqrt(tmp + 1.0 / dotNL2) + sqrt(tmp + 1.0 / dotNV2)));

	// Denominator of GGX distribution term = alpha2/D, where D is the specular distribution of GGX (alpha 2 is multiplied in the end)
	tmp = max(1e-5, (dotNH2 * (alpha2 - 1.0) + 1.0));
	float D_denom = tmp * tmp * PI;

	// Schlick fresnel
	float F = F0 + (1.0 - F0) * pow(1.0 - sqrt(dotNH2), 5.0);
	
	float irradiance = inLight.color * inLight.attenuation * dotNL;
	ioDiffuse += ((1 - F0) / PI) * diffuse_color * irradiance;
	ioSpecular += ((alpha2 * F) / (vis_denom * D_denom)) * specular_color * irradiance;
}


#endif