#include "ShaderDefines.hlsli"

[maxvertexcount(4)]
void main(
	point PARTICLE_VERT_OUT input[1], 
	inout TriangleStream< PARTICLE_VERT_OUT > output
)
{
	float4x4 worldView = mul(worldMatrix, viewMatrix);

	input[0].position = mul(input[0].position, worldView);

	PARTICLE_VERT_OUT verts[4] = 
	{
		float4(-0.05 + input[0].position.x, -0.05 + input[0].position.y, input[0].position.z, 1), float2(0, 1), input[0].color,
		float4(+0.05 + input[0].position.x, -0.05 + input[0].position.y, input[0].position.z, 1), float2(1, 1), input[0].color,
		float4(-0.05 + input[0].position.x, +0.05 + input[0].position.y, input[0].position.z, 1), float2(0, 0), input[0].color,
		float4(+0.05 + input[0].position.x, +0.05 + input[0].position.y, input[0].position.z, 1), float2(1, 0), input[0].color
	};


	for (uint i = 0; i < 4; i++)
	{
		verts[i].position = mul(verts[i].position, projMatrix);
		output.Append(verts[i]);
	}
}