float3 ReconstructCSPosition(float2 inUV, float inLinearDepth, float2 inViewReconstructionVector)
{
	float2 NDC = float2(inUV.x, 1.0-inUV.y) * 2.0 - 1.0;
	return float3(inViewReconstructionVector.xy * NDC * inLinearDepth, -inLinearDepth);
}


half2 AzimuthalEncode(half3 n)
{
	half f = sqrt(2.0 / (1.0 + n.z));
	return n.xy / f;
}


half3 AzimuthalDecode(half2 enc)
{
	half f = dot(enc, enc);
	half g = sqrt(1.0 - f / 4.0);
	half3 n;
	n.xy = enc * g;
	n.z = 1.0 - f / 2.0;
	return n;
}


half2 EncodeNormal(float3 inNormal)
{
	return inNormal.xy;
//	return AzimuthalEncode(inNormal);
}


float3 DecodeNormal(half2 inEncodedNormal)
{
	return float3(inEncodedNormal, sqrt(1.0 - dot(inEncodedNormal, inEncodedNormal)));
//	return AzimuthalDecode(inEncodedNormal);
}


float2 EncodeSphereMap(float3 inNormal)
{
	float oneMinusZ = 1.0f - inNormal.z;
	float p = sqrt(inNormal.x * inNormal.x + inNormal.y * inNormal.y + oneMinusZ * oneMinusZ);
	return inNormal.xy / p * 0.5f + 0.5f;
}


float3 DecodeSphereMap(float2 inEncoded)
{
	float2 tmp = inEncoded - inEncoded * inEncoded;
	float f = tmp.x + tmp.y;
	float m = sqrt(4.0f * f - 1.0f);
	float3 normal;
	normal.xy = m * (inEncoded * 4.0f - 2.0f);
	normal.z = 3.0f - 8.0f * f;
	return normal;
}

