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


uint Hash(uint x) 
{
	x += (x << 10u);
	x ^= (x >> 6u);
	x += (x << 3u);
	x ^= (x >> 11u);
	x += (x << 15u);
	return x;
}
uint Hash(uint2 v) { return Hash(v.x ^ Hash(v.y)); }
uint Hash(uint3 v) { return Hash(v.x ^ Hash(v.y) ^ Hash(v.z)); }
uint Hash(uint4 v) { return Hash(v.x ^ Hash(v.y) ^ Hash(v.z) ^ Hash(v.w)); }


// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float FloatConstruct(uint m) 
{
	const uint ieeeMantissa = 0x007FFFFFu;
	const uint ieeeOne = 0x3F800000u;
	m &= ieeeMantissa;
	m |= ieeeOne;
	float  f = asfloat(m);
	return f - 1.0;
}


// Pseudo-random value in half-open range [0:1].
float Random(float x)	{ return FloatConstruct(Hash(asuint(x))); }
float Random(float2 v)	{ return FloatConstruct(Hash(asuint(v))); }
float Random(float3 v)	{ return FloatConstruct(Hash(asuint(v))); }
float Random(float4 v)	{ return FloatConstruct(Hash(asuint(v))); }


// returns a UV to upsample from
uint2 NearestDepthSelect(uint2 inHiCoord, float4 inLoDepth, float inHiDepth, out bool outSampleBilinear)
{
	float4 delta = abs(inLoDepth - float4(inHiDepth, inHiDepth, inHiDepth, inHiDepth));
	float max_delta = max(max(delta.x, delta.y), max(delta.z, delta.w));

	const float treshold = 0.01;
	if (max_delta < treshold * inHiDepth)
	{
		outSampleBilinear = true;
		return inHiCoord / 2;
	}

	outSampleBilinear = false;
	uint2 coord = (inHiCoord - 1) / 2;

	if (delta.x < delta.y)
	{
		if (delta.z < delta.w)
		{
			if (delta.x < delta.z)
				return coord;				// x
			else
				return coord + uint2(0, 1);	// z
		}
		else
		{
			if (delta.x < delta.w)
				return coord;				// x
			else
				return coord + uint2(1, 1);	// w
		}
	}
	else
	{
		if (delta.z < delta.w)
		{
			if (delta.y < delta.z)
				return coord + uint2(1, 0);	// y
			else
				return coord + uint2(0, 1);	// z
		}
		else
		{
			if (delta.y < delta.w)
				return coord + uint2(1, 0);	// y
			else
				return coord + uint2(1, 1);	// w
		}
	}
}


float ApproximateLuma(float3 inRGB)
{
	return sqrt(0.299 * inRGB.x * inRGB.x + 0.587 * inRGB.y * inRGB.y + 0.114 * inRGB.z * inRGB.z);
}