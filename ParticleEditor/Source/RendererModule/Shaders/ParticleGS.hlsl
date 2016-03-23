#include "ShaderDefines.hlsli"

[maxvertexcount(4)]
void main(
	point PARTICLE_VERT_IN input[1],
	inout TriangleStream< PARTICLE_VERT_OUT > output
	)
{
	float4x4 worldView = mul(worldMatrix, viewMatrix);
		float scale = input[0].scale;
	float s, c;
	sincos(radians(input[0].rot), s, c);
	float4 pos = mul(float4(input[0].position.xyz, 1), worldView);

		PARTICLE_VERT_OUT verts[4] =
	{
		float4(pos.x - scale, pos.y - scale, pos.z, 1), float2(0, 1), input[0].color,
		float4(pos.x + scale, pos.y - scale, pos.z, 1), float2(1, 1), input[0].color,
		float4(pos.x - scale, pos.y + scale, pos.z, 1), float2(0, 0), input[0].color,
		float4(pos.x + scale, pos.y + scale, pos.z, 1), float2(1, 0), input[0].color
	};

	for (uint i = 0; i < 4; i++)
	{
		float x = verts[i].position.x;
		float y = verts[i].position.y;
		verts[i].position.x = (x - pos.x) * c - (y - pos.y) * s + pos.x;
		verts[i].position.y = (x - pos.x) * s + (y - pos.y) * c + pos.y;
		verts[i].position = mul(verts[i].position, projMatrix);
		output.Append(verts[i]);
	}
}