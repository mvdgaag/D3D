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


// TODO: m2*m2 or m2 in CosTheta
// m2*m2 seems to best match analytical light falloff, but needs proof
float3 ImportanceSampleGGX(float2 Xi, float m2, float3 N)
{
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (m2*m2 - 1) * Xi.y));
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


float OrenNayar(float dotNL, float dotNV, float3 N, float V, float m2)
{
	if (dotNL <= 0)
		return 0;

	float a = 1.0 - 0.5 * (m2 / (m2 + 0.57));
	float b = 0.45 * (m2 / (m2 + 0.09));
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


// Schlick Smith G for GGX
float G1_SchlickSmith(float dotXX, float k)
{
	return dotXX / (dotXX * (1.0 - k) + k);
}

float G_SchlickSmithGGX(float dotNL, float dotNV, float m2)
{
	float k = max(m2 * 0.5, 1e-5);
	return G1_SchlickSmith(dotNL, k) * G1_SchlickSmith(dotNV, k);
}


// Smith G for GGX
float G1_GGX(float dotXX, float k)
{
	(2.0 * dotXX) / (dotXX + sqrt(k + (1 - k) * dotXX * dotXX));
}

float G_GGX(float dotNL, float dotNV, float m2)
{
	float k = m2 * m2;
	return G1_GGX(dotNL, k) * G1_GGX(dotNV, k);
}


// smith G with height correlation for GGX
float G1_GGX_Correlated(float dotXX, float k)
{
	float dotXX2 = dotXX * dotXX;
	return (-1.0 + sqrt(k * (1.0 - dotXX2) / dotXX2 + 1.0)) * 0.5;
}

float G_GGX_Correlated(float dotNL, float dotNV, float m2)
{
	float k = m2 * m2;
	return rcp(1.0 + G1_GGX_Correlated(dotNL, k) + G1_GGX_Correlated(dotNV, k));
}


// Smith visibility for GGX, optimized
float RcpV1GGX(float dotXX, float a2)
{
	return dotXX + sqrt(a2 + (1 - a2) * dotXX * dotXX);
}

float V_GGX(float dotNL, float dotNV, float m2)
{
	float a2 = m2*m2;
	return rcp(RcpV1GGX(dotNL, a2) * RcpV1GGX(dotNV, a2));
}


// Smith visibility for GGX, height correlated, optimized
float V_SmithGGXCorrelated(float dotNL, float dotNV, float alphaG)
{
	// Original formulation of G_SmithGGX Correlated
	// lambda_v = (-1 + sqrt(alphaG2 * (1 - dotNL2) / dotNL2 + 1)) / 2;
	// lambda_l = (-1 + sqrt(alphaG2 * (1 - dotNV2) / dotNV2 + 1)) / 2;
	// G_SmithGGXCorrelated = 1 / (1 + lambda_v + lambda_l );
	// V_SmithGGXCorrelated = G_SmithGGXCorrelated / (4 * dotNL * dotNV );
	
	// This is the optimize version
	float alphaG2 = alphaG * alphaG;
	// Caution : the " dotNL *" and " dotNV *" are explicitely inversed , this is not a mistake .
	float Lambda_GGXV = dotNL * sqrt((-dotNV * alphaG2 + dotNV) * dotNV + alphaG2);
	float Lambda_GGXL = dotNV * sqrt((-dotNL * alphaG2 + dotNL) * dotNL + alphaG2);
	
	return 0.5 / (Lambda_GGXV + Lambda_GGXL);
}


/*
**
** Microfacet Distribution functions
**
*/


// GGX Trowbridge-Reitz
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
float D_GGX(float dotNH, float m2)
{
	float tmp = m2 / max(1e-8, (dotNH * dotNH * (m2 * m2 - 1.0) + 1.0));
	return tmp * tmp / PI;
}


/*
**
** Brute-force reference functions
**
*/


float3 BruteForceSpecularIBL(TextureCube EnvMap, SamplerState EnvMapSampler, float3 SpecularColor, float m, float3 N, float3 V)
{
	float3 SpecularLighting = 0;
	const uint kNumSamples = 512;
	float m2 = m*m;
	
	for (uint i = 0; i < kNumSamples; i++)
	{
		float2 Xi = Hammersley(i, kNumSamples);
		float3 H = ImportanceSampleGGX(Xi, m2, N);
		float3 L = 2 * dot(V, H) * H - V;
		
		float dotNL = saturate(dot(N, L));
		if (dotNL > 0)
		{
			float dotNV = saturate(dot(N, V));
			float dotNH = saturate(dot(N, H));
			float dotLH = saturate(dot(L, H));

			// Importance sampling weight for each sample
			// weight = fr * dotNL
			// with : fr = D(H) * F(H) * G(V, L) / ( 4 * dotNL * dotNV )
			
			// Since we integrate in the microfacet space, we include the Jacobian of the transform
			// pdf = D(H) * dotNH / ( 4 * dotLH )
			float D = D_GGX(dotNH, m2);
			float pdf = D * dotNH / (4.0 * dotLH);
		
			// dotNL canceled out
			float3 F = F_GGX(dotLH, SpecularColor, float3(1.0, 1.0, 1.0));
			float G = G_GGX_Correlated(dotNL, dotNV, m2);
			float weight = F * G * D / (4.0 * dotNV);
		
			if (pdf > 0)
				SpecularLighting += EnvMap.SampleLevel(EnvMapSampler, L, 0).rgb * weight / pdf;
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


float2 IntegrateBRDF(float m2, float dotNV)
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
		float3 H = ImportanceSampleGGX(Xi, m2, float3(0.0, 0.0, 1.0));
		float3 L = 2 * dot(V, H) * H - V;

		float dotNL = saturate(L.z);
		float dotNH = saturate(H.z);
		float dotVH = saturate(dot(V, H));
	
		if (dotNL > 0)
		{
			float G = G_GGX_Correlated(dotNL, dotNV, m2);
			float vis = G * dotVH / (dotNH * dotNV);
			float Fc = pow(1 - dotVH, 5);
			A += (1 - Fc) * vis;
			B += Fc * vis;
		}
	}

	return float2(A, B) / kNumSamples;
}


float3 PrefilterEnvMap(TextureCube EnvMap, SamplerState EnvMapSampler, float m2, float3 R)
{
	float3 N = R;
	float3 V = R;
	float3 prefiltered_color = 0;
	float  total_weight = 0;
	const uint kNumSamples = 256;

	for (uint i = 0; i < kNumSamples; i++)
	{
		float2 Xi = Hammersley(i, kNumSamples);
		float3 H = ImportanceSampleGGX(Xi, m2, N);
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


// Warning expensive version without lookup textures or prefiltered mips on cubemap!
float3 ApproximateSpecularIBL(TextureCube EnvMap, SamplerState EnvMapSampler, float3 SpecularColor, float m, float3 N, float3 V)
{
	float m2 = m * m;

	float dotNV = saturate(dot(N, V));
	float3 R = 2 * dot(V, N) * N - V;

	// HACK attempt to approximate sampling the dominant specular direction
	R = lerp(R, N, m*m2);

	float3 prefiltered_color = PrefilterEnvMap(EnvMap, EnvMapSampler, m2, R);
	float2 env_brdf = IntegrateBRDF(m2, dotNV);
	
	return prefiltered_color * (SpecularColor * env_brdf.x + env_brdf.y);
}


float3 ApproximateSpecularIBL(TextureCube EnvMap, SamplerState EnvMapSampler, float EnvMapMaxMip, Texture2D BRDFLookupTexture, SamplerState BRDFLookupSampler, float3 SpecularColor, float m, float3 N, float3 V)
{
	float m2 = m * m;

	float dotNV = saturate(dot(N, V));
	float3 R = 2 * dot(V, N) * N - V;

	// HACK attempt to approximate sampling the dominant specular direction
	R = lerp(R, N, m*m2);

	float mip = m * (EnvMapMaxMip - 2);

	float3 prefiltered_color = EnvMap.SampleLevel(EnvMapSampler, R, mip);
	float2 env_brdf = BRDFLookupTexture.SampleLevel(BRDFLookupSampler, float2(m2, dotNV), 0);

	return prefiltered_color * (SpecularColor * env_brdf.x + env_brdf.y);
}


/*
**
** Lighting functions
**
*/


/*
// http://www.frostbite.com/2014/11/moving-frostbite-to-pbr/
void SpherelightIlluminance(float3 lightWorldPos, float3 worldPos, float3 worldNormal, float radius)
{
	float3 Lunormalized = lightWorldPos - worldPos;
	float3 L = normalize(Lunormalized);
	float distance2 = dot(Lunormalized, Lunormalized);
	float illuminance = 0;

#if WITHOUT_CORRECT_HORIZON // Analytical solution above horizon
	// Patch to Sphere frontal equation ( Quilez version )
	float radius2 = radius * radius;
	// Do not allow object to penetrate the light ( max )
	// Form factor equation include a (1 / PI ) that need to be cancel
	// thus the " PI *"
	illuminance = PI * (radius2 / (max(radius2, distance2))) * saturate(dot(worldNormal, L));
# else // Analytical solution with horizon
	// Tilted patch to sphere equation
	float beta = acos(dot(worldNormal, L));
	float H = sqrt(distance2);
	float h = H / radius;
	float x = sqrt(h * h - 1);
	float y = -x * (1 / tan(beta));
	
	if (h * cos(beta) > 1)
		illuminance = cos(beta) / (h * h);
	else
		illuminance = (1 / (PI * h * h)) * (cos(beta) * acos(y) - x * sin(beta) * sqrt(1 - y * y)) +	(1 / PI) * atan(sin(beta) * sqrt(1 - y * y) / x);
	
	illuminance *= PI;
# endif

	return illuminance;
}



	cos_b = dot(worldNormal, L);
	sin_b = sqrt(1 - cos_b * cos_b);
	tan_b = sin_b / cos_b;

	// Tilted patch to sphere equation
	
	float distance = sqrt(distance2);
	float norm_distance = distance / radius;
	float norm_distance2 = norm_distance * norm_distance;

	float edge_distance = sqrt(norm_distance2 - 1);
	float y = -edge_distance * (1 / tan_b);
	float y2 = y*y;

	if (h * cos_b > 1)
		illuminance = cos_b / (norm_distance2);
	else
		illuminance = (1 / (PI * norm_distance2)) * 
			(cos_b * acos(y) - edge_distance * sin_b * sqrt(1 - y2)) + 
			(1 / PI) * atan(sin_b * sqrt(1 - y2) / edge_distance);

	illuminance *= PI;
*/


void BRDF_GGX(float3 N, float3 V, float3 L, float m2, float3 F0, out float3 Spec, out float3 Diff)
{
	float3 H = normalize(V + L);

	float dotNL = saturate(dot(N, L));
	float dotNV = saturate(dot(N, V));
	float dotNH = saturate(dot(N, H));
	float dotLH = saturate(dot(L, H));

	float D = D_GGX(dotNH, m2);
	float F = F_GGX(dotLH, F0, float3(1.0, 1.0, 1.0));
	float vis = V_SmithGGXCorrelated(dotNL, dotNV, m2);

	Spec = D * F * vis; // = D * F * G / (4 * dotNL * dotNV)
	Diff = (1 - F) / PI;
}


void AccumulateLight(Material inMaterial, float3 inPosition, float3 inNormal, Light inLight, inout float3 ioDiffuse, inout float3 ioSpecular)
{
	float3 N =			inNormal;
	float3 V =			normalize(-inPosition);
	float3 L =			inLight.direction;
	float m =			max(0.1, saturate(inMaterial.roughness));
	float m2 =			m * m;
	float F0 =			max(0.02, saturate(inMaterial.reflectance));

	float3 specular = lerp(float3(F0, F0, F0), (F0 * 2 + 0.5) * inMaterial.diffuse.rgb, inMaterial.metalicity);
	float3 diffuse = inMaterial.diffuse.rgb * (1 - inMaterial.metalicity);
	
	// TODO: this is double calculated in the brdf function!
	float dotNL = saturate(dot(N, L));

	float3 S, D;
	BRDF_GGX(N, V, L, m2, specular, S, D);
	
	ioDiffuse += D * diffuse * inLight.color * inLight.attenuation * dotNL;
	ioSpecular += S * specular * inLight.color * inLight.attenuation * dotNL;
}


#endif