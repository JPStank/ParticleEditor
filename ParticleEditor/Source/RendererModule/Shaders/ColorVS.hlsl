#include "ShaderDefines.hlsli"

COLOR_VERT_OUT main(COLOR_VERT_IN input)
{
	COLOR_VERT_OUT output = (COLOR_VERT_OUT)0;

	float4 pos = float4(input.position, 1);
	pos = mul(pos, worldMatrix);
	pos = mul(pos, viewMatrix);
	pos = mul(pos, projMatrix);

	output.position = pos;
	output.color = input.color;

	return output;
}