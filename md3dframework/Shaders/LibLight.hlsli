#define PI 3.141572


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


float OrenNayar(float dotNL, float dotNV, float3 N, float V, float m2)
{
	if (dotNL <= 0)
		return 0;

	float a = 1.0 - 0.5 * (m2 / (m2 + 0.57));
	float b = 0.45 * (m2 / (m2 + 0.09));
	float ga = dot(V - N * dotNV, N - N * dotNL);
	return dotNL * (a + b * max(0.0, ga) * sqrt((1.0 - dotNV * dotNV) * (1.0 - dotNL * dotNL)) / max(dotNL, dotNV));
}


float3 F_GGX(float dotLH, float F0)
{
	float dotLH5 = pow(1.0f - dotLH, 5);
	return F0 + (1.0 - F0) * (dotLH5);
}


float G1V(float dotXX, float m2)
{
	return 1.0f / (dotXX + sqrt(m2 + (1 - m2) * dotXX * dotXX));
}


float G_GGX(float dotNL, float dotNV, float m2)
{
	return G1V(dotNL, m2) * G1V(dotNV, m2);
}


float D_GGX(float dotNH, float m2)
{
	float m2Sqr = m2 * m2;
	float denom = dotNH * dotNH * (m2Sqr - 1.0) + 1.0f;
	return m2Sqr / (denom * denom); // Division by PI left out (applied later)
}


float D_GGX_Anisotropic(float dotNH, float dotXH, float dotYH, float m2, float anisotropy)
{
	float dotXH2 = dotXH * dotXH;
	float dotYH2 = dotYH * dotYH;
	float dotNH2 = dotNH * dotNH;
	float mx = m2;
	float my = lerp(0, mx, 1.0f - anisotropy);
	float mx2 = mx * mx;
	float my2 = my * my;
	float denom = dotXH2 / mx2 + dotYH2 / my2 + dotNH2;
	return (1.0f / (mx * my)) * (1.0 / (denom * denom)); // Division by PI left out (applied later)
}


void LightingFuncGGX(float3 N, float3 V, float3 L, float m, float F0, out float Spec, out float Diff)
{
	float m2 = m*m;

	float3 H = normalize(V + L);

	float dotNL = saturate(dot(N, L));
	float dotNV = saturate(dot(N, V));
	float dotNH = saturate(dot(N, H));
	float dotLH = saturate(dot(L, H));

	float D = D_GGX(dotNH, m2);
	float F = F_GGX(dotLH, F0);
	float G = G_GGX(dotNL, dotNV, m2);
	
	Spec = dotNL * D * F * G;
	Diff = (1.0 - F) * dotNL;
}


void AccumulateLight(Material inMaterial, float3 inPosition, float3 inNormal, Light inLight, inout float3 ioDiffuse, inout float3 ioSpecular)
{
	float3 N =			inNormal;
	float3 V =			normalize(-inPosition);
	float3 L =			inLight.direction;
	float m =			max(0.025, saturate(inMaterial.roughness));
	float F0 =			max(0.025, saturate(inMaterial.reflectance));
	float3 diffuse =	inMaterial.diffuse.rgb * (1 - inMaterial.metalicity);
	float3 specular =	lerp(float3(1.0, 1.0, 1.0), inMaterial.diffuse.rgb, inMaterial.metalicity);
	
	//DEVHACK add roughness to keep specular intact over distance
	//m = saturate(m + lerp(0.0, 0.05, saturate(-inPosition.z * 0.01)));

	float S, D;
	LightingFuncGGX(N, V, L, m, F0, S, D);
	
	ioDiffuse += D * diffuse * inLight.color * inLight.attenuation / PI;
	ioSpecular += S * specular * inLight.color * inLight.attenuation / PI;
}
