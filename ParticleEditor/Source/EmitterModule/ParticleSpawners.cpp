/***********************************************
* Filename:           	ParticleSpawners.h
* Date :				02 / 09 / 2016
* Mod.Date :			02 / 09 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Define the different ways
				that a particle can be spawned into
				a given system
************************************************/
#include "ParticleSpawners.h"
#include "ParticleSystem.h"

CBoxSpawner::CBoxSpawner()
{
	m_d3dCenter = { 0, 0, 0 };
	m_d3dOffset = { 1, 1, 1 };
}
CBoxSpawner::CBoxSpawner(float3 center, float3 offset)
{
	m_d3dCenter = center;
	m_d3dOffset = offset;
}
CBoxSpawner::~CBoxSpawner()
{
}

void CBoxSpawner::Spawn(float dt, CParticleSystem* sys, unsigned int start, unsigned int end)
{
	float3 min = { m_d3dCenter.x - m_d3dOffset.x, m_d3dCenter.y - m_d3dOffset.y, m_d3dCenter.z - m_d3dOffset.z };
	float3 max = { m_d3dCenter.x + m_d3dOffset.x, m_d3dCenter.y + m_d3dOffset.y, m_d3dCenter.z + m_d3dOffset.z };

	for (unsigned int i = start; i < end; ++i)
	{
		sys->GetPosition()[i] = float4{
			RAND_FLOAT(min.x, max.x),
			RAND_FLOAT(min.y, max.y),
			RAND_FLOAT(min.z, max.z),
			1.0f
		};
	}
}

CSphereVelocitySpawner::CSphereVelocitySpawner()
{
	m_fSpeed = 2.0f;
}
CSphereVelocitySpawner::CSphereVelocitySpawner(float speed)
{
	m_fSpeed = speed;
}
CSphereVelocitySpawner::~CSphereVelocitySpawner()
{
}

void CSphereVelocitySpawner::Spawn(float dt, CParticleSystem* sys, unsigned int start, unsigned int end)
{
	float phi, theta, r;
	for (unsigned int i = start; i < end; ++i)
	{
		//phi = RAND_FLOAT(0, DirectX::XM_2PI);
		//theta = RAND_FLOAT(0, DirectX::XM_2PI);
		phi = RAND_FLOAT(-DirectX::XM_PI, DirectX::XM_PI);
		theta = RAND_FLOAT(-DirectX::XM_PI, DirectX::XM_PI);
		r = m_fSpeed * sinf(phi);

		sys->GetVelocity()[i].x = m_fSpeed * cosf(phi);
		sys->GetVelocity()[i].y = r * cosf(theta);
		sys->GetVelocity()[i].z = r * sinf(theta);
	}
}

CUniformTimeSpawner::CUniformTimeSpawner()
{
	m_fDuration = 5.0f;
}
CUniformTimeSpawner::CUniformTimeSpawner(float duration)
{
	m_fDuration = duration;
}
CUniformTimeSpawner::~CUniformTimeSpawner()
{
}

void CUniformTimeSpawner::Spawn(float dt, CParticleSystem* sys, unsigned int start, unsigned int end)
{
	for (unsigned int i = start; i < end; ++i)
	{
		sys->GetTime()[i].x = sys->GetTime()[i].y = m_fDuration;
		sys->GetTime()[i].z = 0.0f; // will be used to keep interpolated value
		sys->GetTime()[i].w = 1.0f / m_fDuration; // used to calculate
	}
}
