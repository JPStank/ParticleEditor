/***********************************************
* Filename:           	RenderDefines.h
* Date:  	         	02/06/2016
* Mod. Date:    	   	02/09/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Define structs, classes,
*               	typedefs, functions and includes
*					common for all render files
************************************************/
#ifndef _RENDER_DEFINES_H_
#define _RENDER_DEFINES_H_

// comment this out if you don't want a grid to render
//#define RENDER_GRID
// comment this out to remove FPS display
#define SHOW_FPS

/**********
* INCLUDES
**********/

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "../DXLog.h"
#include "../Log.h"
#include "../LevelModule/Object.h"

// used for camera initialized in renderer
#define FOV 60.0f
#define ZNEAR 0.1f
#define ZFAR 100.0f
#define NUM_BONES 50

/**********
* TYPEDEFS cause i'm lazy
**********/

typedef DirectX::XMFLOAT2 float2;
typedef DirectX::XMFLOAT3 float3;
typedef DirectX::XMFLOAT4 float4;
typedef DirectX::XMUINT4 uint4;
typedef DirectX::XMFLOAT4X4 float4x4;

/**********
* STRUCTS
**********/

// used for debug shapes and lines
struct TColorVert
{
	float3 m_d3dPosition;
	float4 m_d3dColor;
};

// used for simple mesh types
struct TStaticMeshVert
{
	float3 m_d3dPosition;
	float2 m_d3dTexCoord;
	float3 m_d3dNormal;
};

// used for particle rendering
struct TParticleVert
{
	float4 m_d3dPosition;
	float4 m_d3dColor;
	//float  m_fScale;
	//float  m_fRotation;
};

// used for animated skinned meshes
struct TSkinnedVert
{
	float3 m_d3dPosition;
	float2 m_d3dTexCoord;
	float3 m_d3dNormal;
	uint4 m_d3dBoneIndex;
	float4 m_d3dBoneWeight;
};

// used for screen space objects
struct THudVert
{
	float3 m_d3dPosition;
	float4 m_d3dColor;
	float2 m_d3dTexCoord;
};

// used for mapping camera and projection to cbuffer
// register(b1)
struct TPerFrame
{
	float4x4 m_d3dViewMatrix;
	float4x4 m_d3dProjMatrix;
};

// used for mapping light information to cbuffer
// register(b3)
struct TLightInfo
{
	//not needed since everything is view space
	//float3 m_d3dEyePos;
	DirectX::XMUINT4 m_d3dNumLights;
};

// used for light structured buffer
struct TLight
{
	float4 m_d3dPosition;
	float4 m_d3dDirection;
	float4 m_d3dPosVS;
	float4 m_d3dDirVS;
	float4 m_d3dColor;
	float m_fSpotLightAngle;
	float m_fRange;
	float m_fIntensity;
	UINT m_eLightType;
};

/**********
* CLASSES
**********/

// simple object for containing all buffers to a render component
// these are what ultimately get stored within a render context
class CRenderObject
{
public:
	CRenderObject()
	{
		ZeroMemory(this, sizeof(CRenderObject));
	}

	CObject* m_pobjOwner;

	ID3D11Buffer*	m_pd3dVertexBuffer;
	ID3D11Buffer*	m_pd3dIndexBuffer;
	ID3D11Buffer*	m_pd3dMatrixBuffer;
	// TODO: Add more items as they reveal themselves

	TParticleVert*	m_ptParticleData;
};

class IUIElement;
// special render object for holding onto UI information
class CUIRenderObject
{
public:
	CUIRenderObject()
	{
		ZeroMemory(this, sizeof(CUIRenderObject));
	}

	ID3D11Buffer*	m_pd3dVertexBuffer;
	ID3D11Buffer*	m_pd3dIndexBuffer;

	IUIElement*		m_pUIElement;
};

#endif//_RENDER_DEFINES_H_