/***********************************************
* Filename:           	ParticleSystem.cpp
* Date :				02 / 09 / 2016
* Mod.Date :			03 / 08 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Component that holds onto
the raw data of particles needing
to be rendered and emitter properties
************************************************/
#include "ParticleSystem.h"
#include "ParticleSpawners.h"
#include "ParticleUpdaters.h"

CParticleSystem::CParticleSystem()
{
}
CParticleSystem::CParticleSystem(unsigned int maxCount, float rate)
{
	m_unMaxCount = maxCount;
	m_fEmitRate = rate;
	m_unAliveCount = 0;
	m_fTimer = 0.0f;

	m_vbAlive.resize(maxCount);
	m_vd3dPositions.resize(maxCount);
	m_vd3dColor.resize(maxCount);
	m_vd3dVelocity.resize(maxCount);
	m_vd3dTime.resize(maxCount);
	m_vfScale.resize(maxCount);
	m_vfRotation.resize(maxCount);

	// maybe add more defaults if needed
	for (unsigned int i = 0; i < maxCount; ++i)
	{
		m_vbAlive[i] = false;
		m_vd3dColor[i] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_vfScale[i] = 1.0f;
		m_vfRotation[i] = 0.0f;
	}
}
CParticleSystem::~CParticleSystem()
{
	m_vbAlive.clear();
	m_vd3dColor.clear();
	m_vd3dPositions.clear();
	m_vd3dVelocity.clear();
	m_vd3dTime.clear();
	m_vfScale.clear();
	m_vfRotation.clear();
}
IComponent::eComponentType CParticleSystem::GetType()
{
	return eEMITTER;
}
void CParticleSystem::Kill(unsigned int id)
{
	if (m_unAliveCount > 0)
	{
		m_vbAlive[id] = false;
		SwapData(id, m_unAliveCount - 1);
		--m_unAliveCount;
	}
}
void CParticleSystem::Wake(unsigned int id)
{
	if (m_unAliveCount < m_unMaxCount)
	{
		m_vbAlive[id] = true;
		SwapData(id, m_unAliveCount);
		++m_unAliveCount;
	}
}
void CParticleSystem::SwapData(unsigned int a, unsigned int b)
{
	std::swap(m_vbAlive[a], m_vbAlive[b]);
	std::swap(m_vd3dColor[a], m_vd3dColor[b]);
	std::swap(m_vd3dPositions[a], m_vd3dPositions[b]);
	std::swap(m_vd3dVelocity[a], m_vd3dVelocity[b]);
	std::swap(m_vd3dTime[a], m_vd3dTime[b]);
	std::swap(m_vfScale[a], m_vfScale[b]);
	std::swap(m_vfRotation[a], m_vfRotation[b]);
}
std::vector<DirectX::XMFLOAT4>& CParticleSystem::GetPosition()
{
	return m_vd3dPositions;
}
std::vector<DirectX::XMFLOAT4>& CParticleSystem::GetColor()
{
	return m_vd3dColor;
}
std::vector<DirectX::XMFLOAT4>& CParticleSystem::GetVelocity()
{
	return m_vd3dVelocity;
}
std::vector<DirectX::XMFLOAT4>& CParticleSystem::GetTime()
{
	return m_vd3dTime;
}
std::vector<float>& CParticleSystem::GetScale()
{
	return m_vfScale;
}
std::vector<float>& CParticleSystem::GetRotation()
{
	return m_vfRotation;
}
unsigned int CParticleSystem::GetAlive() const
{
	return m_unAliveCount;
}
unsigned int CParticleSystem::GetMax() const
{
	return m_unMaxCount;
}
float CParticleSystem::GetRate() const
{
	return m_fEmitRate;
}
void CParticleSystem::Update(float dt)
{
	unsigned int numCreate;
	float t = dt *m_fEmitRate;
	if (t < 1.0f)
	{
		m_fTimer += t;
		if (m_fTimer > 1.0f)
		{
			m_fTimer -= 1.0f;
			numCreate = 1;
		}
		else
		{
			numCreate = 0;
		}
	}
	else
	{
		numCreate = (unsigned int)t;
	}

	unsigned int start = m_unAliveCount;
	unsigned int end = std::min(start + numCreate, m_unMaxCount - 1);

	for each (IParticleSpawners* spawn in m_vpSpawners)
	{
		spawn->Spawn(this, start, end);
	}

	for (unsigned int i = start; i < end; ++i)
	{
		Wake(i);
	}

	for each (IParticleUpdater* update in m_vpUpdaters)
	{
		update->Update(dt, this);
	}
}
std::vector<IParticleSpawners*>& CParticleSystem::GetSpawners()
{
	return m_vpSpawners;
}
std::vector<IParticleUpdater*>&  CParticleSystem::GetUpdaters()
{
	return m_vpUpdaters;
}
