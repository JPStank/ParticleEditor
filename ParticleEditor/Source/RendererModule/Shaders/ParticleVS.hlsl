#include "ShaderDefines.hlsli"

PARTICLE_VERT_OUT main(PARTICLE_VERT_IN input)
{
	PARTICLE_VERT_OUT output = (PARTICLE_VERT_OUT)0;
	
	output.position = float4(input.position.xyz, 1);
	output.color = input.color;
	
	return output;
}