/***********************************************
* Filename:           	ParticleDefines.h
* Date :				02 / 09 / 2016
* Mod.Date :			02 / 09 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Standard includes and defines
				that may be needed by particle systems
************************************************/
#ifndef _PARTICLE_DEFINES_H_
#define _PARTICLE_DEFINES_H_

#include <DirectXMath.h>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <vector>


#define RAND_FLOAT(min, max)  (min + (float)(rand())/(float)(RAND_MAX/(max - min)))
#define LERP(start, end, r) (start + r * (end - start))

/**********
* TYPEDEFS cause i'm lazy
**********/

typedef DirectX::XMFLOAT2 float2;
typedef DirectX::XMFLOAT3 float3;
typedef DirectX::XMFLOAT4 float4;
typedef DirectX::XMFLOAT4X4 float4x4;


#endif//_PARTICLE_DEFINES_H_