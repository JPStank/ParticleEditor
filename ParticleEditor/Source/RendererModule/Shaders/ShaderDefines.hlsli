/***********************************************
* Filename:           	ShaderDefines.hlsli
* Date:  	         	02/06/2016
* Mod. Date:    	   	02/06/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Structs and cBuffers that
					are to be shared across shaders
************************************************/
#ifndef _SHADER_DEFINES_HLSLI_
#define _SHADER_DEFINES_HLSLI_

#pragma pack_matrix(row_major)

#define NUM_BONES 50

#define DIR_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

cbuffer WorldMatrix : register(b0)
{
	float4x4 worldMatrix;
}

cbuffer ViewProjBuffer : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projMatrix;
}

cbuffer InvBindBones : register(b2)
{
	float4x4 animatedBones[NUM_BONES];
}

cbuffer LightInfo : register(b3)
{
	// not needed due to eye always at origin
	//float3 eyePos;
	uint4 numLights; // for simplicity sake
}

struct Light
{
	float4 position;
	float4 direction;
	float4 posVS;
	float4 dirVS;
	float4 color;
	float SpotLightAngle;
	float Range;
	float Intensity;
	uint LightType;
};

StructuredBuffer<Light> myLights : register(t8);

struct COLOR_VERT_IN
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct COLOR_VERT_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct STATIC_VERT_IN
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 norm : NORMAL;
};

struct STATIC_VERT_OUT
{
	float4 position : SV_POSITION;
	float3 posVS : VIEW_POS;
	float2 uv : TEXCOORD;
	float3 norm : NORMAL;
	float4 worldPos : WORLD_POS;
};

struct PARTICLE_VERT_IN
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PARTICLE_VERT_OUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
};

struct ANIMATION_VERT_IN
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 norm : NORMAL;
	uint4 bone : BONE_INDEX;
	float4 weight : BONE_WEIGHT;
};

struct HUD_VERT_IN
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct HUD_VERT_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

#endif//_SHADER_DEFINES_HLSLI_