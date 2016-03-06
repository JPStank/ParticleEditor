#include "ShaderDefines.hlsli"

float4 main(COLOR_VERT_OUT input) : SV_TARGET
{
	return input.color;
}