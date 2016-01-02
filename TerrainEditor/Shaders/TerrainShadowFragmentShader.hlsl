struct PS_INPUT
{
	float4 Position	: SV_POSITION;
};


struct PS_OUTPUT
{
	float Depth		: SV_Target0;
};


PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;
	output.Depth = input.Position.z;
	return output;
}
