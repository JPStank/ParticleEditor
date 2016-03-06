/***********************************************
* Filename:           	EmitterManager.cpp
* Date :				02 / 10 / 2016
* Mod.Date :			02 / 10 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Manage all particle systems in
				the scene and pass along the update
				to each one
************************************************/
#include "EmitterManager.h"
#include "ParticleSystem.h"
#include "ParticleSpawners.h"
#include "ParticleUpdaters.h"

#include "../LevelModule/Object.h"
#include "../Log.h"

CEmitterManager::CEmitterManager()
{
}
CEmitterManager::~CEmitterManager()
{
}
bool CEmitterManager::Initialize(void)
{

	return true;
}
bool CEmitterManager::ShutDown(void)
{
	for (int i = 0; i < (int)m_vpSystems.size(); ++i)
	{
		UnregisterParticleSystem(m_vpSystems[i]);
	}

	delete m_pobjTestSystem;

	return true;
}
void CEmitterManager::Update(float dt)
{
	for each (CParticleSystem* sys in m_vpSystems)
	{
		sys->Update(dt);
	}
}
void CEmitterManager::RegisterParticleSystem(CParticleSystem* sys)
{
	m_vpSystems.push_back(sys);
}
void CEmitterManager::UnregisterParticleSystem(CParticleSystem* sys)
{
	auto iter = m_vpSystems.begin();
	auto end = m_vpSystems.end();

	while (iter != end)
	{
		CParticleSystem* s = *iter;
		if (s = sys)
		{
			m_vpSystems.erase(iter);
			return;
		}
		++iter;
	}

	Log("Could not remove object from Emitter Manager");
}
