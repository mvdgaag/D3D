#define PI 3.141572


struct Material
{
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




float OrenNayar(float dotNL, float dotNV, float3 N, float V, float alpha)
{
	if (dotNL <= 0)
		return 0;

	float a = 1.0 - 0.5 * (alpha / (alpha + 0.57));
	float b = 0.45 * (alpha / (alpha + 0.09));
	float ga = dot(V - N * dotNV, N - N * dotNL);
	return dotNL * (a + b * max(0.0, ga) * sqrt((1.0 - dotNV * dotNV) * (1.0 - dotNL * dotNL)) / max(dotNL, dotNV));
}

float3 F_GGX(float dotLH, float F0)
{
	float dotLH5 = pow(1.0f - dotLH, 5);
	return F0 + (1.0 - F0) * (dotLH5);
}

float G1V(float dotXX, float k)
{
	return 1.0f / (dotXX * (1.0f - k) + k);
}

float V_GGX(float dotNL, float dotNV, float alpha)
{
	float k = alpha / 2.0f;
	return G1V(dotNL, k) * G1V(dotNV, k);
}

float D_GGX(float dotNH, float alpha)
{
	float alphaSqr = alpha * alpha;
	float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0f;
	return alphaSqr / (denom * denom); // Division by PI left out (applied later)
}

float D_GGX_Anisotropic(float dotNH, float dotXH, float dotYH, float alpha, float anisotropy)
{
	float dotXH2 = dotXH * dotXH;
	float dotYH2 = dotYH * dotYH;
	float dotNH2 = dotNH * dotNH;
	float mx = alpha;
	float my = lerp(0, mx, 1.0f - anisotropy);
	float mx2 = mx * mx;
	float my2 = my * my;
	float denom = dotXH2 / mx2 + dotYH2 / my2 + dotNH2;
	return (1.0f / (mx * my)) * (1.0 / (denom * denom)); // Division by PI left out (applied later)
}

void LightingFuncGGX(float3 N, float3 V, float3 L, float roughness, float F0, out float Spec, out float Diff)
{
	float alpha = roughness*roughness;

	float3 H = normalize(V + L);

	float dotNL = saturate(dot(N, L));
	float dotNV = saturate(dot(N, V));
	float dotNH = saturate(dot(N, H));
	float dotLH = saturate(dot(L, H));

	float D = D_GGX(dotNH, alpha);

	/*
	float3 x = float3(1.0, 0.0, 0.0);
	float3 y = normalize(cross(N, x));
	x = normalize(cross(N, y));
	float dotXH = saturate(dot(x, H));
	float dotYH = saturate(dot(y, H));
	D = D_GGX_Anisotropic(dotNH, dotXH, dotYH, alpha, 0.9);
	*/

	float F = F_GGX(dotLH, F0);
	float vis = V_GGX(dotNL, dotNV, alpha);
	
	Spec = dotNL * D * F * vis;
	Diff = (1.0 - F) * OrenNayar(dotNL, dotNV, N, V, 1.0 - roughness);
}

void AccumulateLight(Material inMaterial, float3 inPosition, float3 inNormal, Light inLight, inout float3 ioDiffuse, inout float3 ioSpecular)
{
	float3 N =			inNormal;
	float3 V =			normalize(-inPosition);
	float3 L =			inLight.direction;
	float roughness =	saturate(inMaterial.roughness * inMaterial.roughness * 0.9998 + 0.0001);
	float F0 =			saturate(inMaterial.reflectance * inMaterial.reflectance * 0.9998 + 0.0001);
	

	//DEVHACK add roughness to keep specular intact over distance
	roughness = saturate(roughness + lerp(0.0, 0.05, saturate(-inPosition.z * 0.01)));


	float S, D;

	LightingFuncGGX(N, V, L, roughness, F0, S, D);
	ioDiffuse += D * inMaterial.diffuse * inLight.color * inLight.attenuation / PI;
	ioSpecular += S * inLight.color * inLight.attenuation / PI;

	float3 ambient_color = float3(0.1, 0.1, 0.1);
	ioDiffuse += inMaterial.diffuse * ambient_color * (1.0 - F0) / PI;
}
