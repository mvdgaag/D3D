half2 EncodeNormal(float3 inNormal)
{
	half2 enc = normalize(inNormal.xy) * (sqrt(-inNormal.z*0.5 + 0.5));
	return enc * 0.5 + 0.5;
}


float3 DecodeNormal(half2 inEncodedNormal)
{
	float3 normal;
	float2 fenc = inEncodedNormal * 2 - 1;
	normal.z = (dot(fenc, fenc) * 2 - 1);
	normal.xy = normalize(fenc) * sqrt(1 - normal.z * normal.z);
	return normal;
}