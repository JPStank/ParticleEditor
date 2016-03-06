/***********************************************
* Filename:           	EmitterManager.h
* Date :				02 / 10 / 2016
* Mod.Date :			02 / 10 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Manage all particle systems in
				the scene and pass along the update
				to each one
************************************************/
#ifndef _EMITTER_MANAGER_H_
#define _EMITTER_MANAGER_H_

#include <vector>

class CParticleSystem;
class CObject;

class CEmitterManager
{
public:
	CEmitterManager();
	~CEmitterManager();

	bool Initialize(void);
	bool ShutDown(void);
	void Update(float dt);
	void RegisterParticleSystem(CParticleSystem* sys);
	void UnregisterParticleSystem(CParticleSystem* sys);

private:
	std::vector<CParticleSystem*>	m_vpSystems;
	CObject*						m_pobjTestSystem;
};

#endif//_EMITTER_MANAGER_H_