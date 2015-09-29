half2 AzimuthalEncodeNormal(float3 inNormal)
{
	half f = sqrt(8.0 * inNormal.z + 8.0);
	return inNormal.xy / f + 0.5;
}


half3 AzimuthalDecodeNormal(half2 inEncodedNormal)
{
	half2 fenc = inEncodedNormal * 4.0 - 2.0;
		half f = dot(fenc, fenc);
	half g = sqrt(1.0 - f / 4.0);
	half3 normal = half3(fenc * g, 1.0 - f / 2.0);
		return normal;
}