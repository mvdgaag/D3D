

// TODO: also add prevHeightmap for correct motion vectors
Texture2D cHeightTexture : register(t0);
SamplerState cHeightSampler : register(s0);

cbuffer cEveryFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewProjectionMatrix;
	matrix inverseProjectionMatrix;
	float4 frameData;								// jitter_offset.xy, frameID, 0
};

cbuffer cObject : register(b1)
{
	matrix modelViewMatrix;
	matrix modelViewProjectionMatrix;
	matrix prevModelViewProjectionMatrix;
};

cbuffer cTerrain : register(b2)
{
	float4 cTerrainScale;
};


struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent	: TANGENT;
	float2 TexCoord : TEXCOORD;
};


struct PS_INPUT
{
	float4 Position			: SV_POSITION;
	float3 Normal			: NORMAL;
	float3 Tangent			: TANGENT;
	float2 TexCoord			: TEXCOORD0;
	float2 MotionVectors	: TEXCOORD1;
	float  LinearDepth		: TEXCOORD2;
};



PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output =		(PS_INPUT)0;

	float3 pos = input.Position;
	
		// no displace
		//pos.y += cTerrainScale.z - 0.125 * cHeightTexture.SampleLevel(cHeightSampler, input.TexCoord, 0).x;
	

	// jitter for TAA
	output.Position = mul(float4(pos, 1.0), modelViewProjectionMatrix);
	output.Normal = mul(float4(input.Normal, 0.0), modelViewMatrix).xyz;
	output.Tangent = mul(float4(input.Tangent, 0.0), modelViewMatrix).xyz;
	output.TexCoord = input.TexCoord.xy;

	float4 cam_space_pos = mul(float4(pos, 1.0), (modelViewMatrix));
	output.LinearDepth = cam_space_pos.z;
	
	// motion vectors
	float4 prevPos = mul(float4(pos, 1.0), prevModelViewProjectionMatrix);
	output.MotionVectors = (output.Position.xy / output.Position.w); // currentNDC
	output.MotionVectors -= (prevPos.xy / prevPos.w); // prevNDC
	output.MotionVectors *= 0.5; // NDC to UV range conversion

	// jitter for TAA
	float2 jitter_offset = frameData.xy;
	output.Position.xy += jitter_offset.xy * output.Position.w;

	return output;
}