#include "ShaderDefines.hlsli"

float4 main(PARTICLE_VERT_OUT input) : SV_TARGET
{
	return input.color;// float4(1.0f, 1.0f, 1.0f, 1.0f);
}