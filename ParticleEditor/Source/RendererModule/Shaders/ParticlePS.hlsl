#include "ShaderDefines.hlsli"

SamplerState filter[2] : register(s0); // [0] == CLAMP - [1] == WRAP

float4 main(PARTICLE_VERT_OUT input) : SV_TARGET
{
	float4 texColor = diffuseTexture.Sample(filter[0], input.uv);
	if (texColor.a < 0.01)
		discard;

	return input.color * texColor;
}