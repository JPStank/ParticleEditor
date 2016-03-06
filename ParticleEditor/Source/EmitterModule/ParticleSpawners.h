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
#ifndef _PARTICLE_SPAWNERS_H_
#define _PARTICLE_SPAWNERS_H_

#include "../Interface.h"
#include "ParticleDefines.h"

class CParticleSystem;

CREATE_INTERFACE(IParticleSpawners)
{
	IParticleSpawners() {}
	virtual ~IParticleSpawners() {}

	virtual void Spawn(float dt, CParticleSystem* sys, unsigned int start, unsigned int end) = 0;
};

class CBoxSpawner : public IParticleSpawners
{
public:
	CBoxSpawner();
	~CBoxSpawner();
	CBoxSpawner(float3 center, float3 offset);

	virtual void Spawn(float dt, CParticleSystem* sys, unsigned int start, unsigned int end) override;

private:
	float3	m_d3dCenter;
	float3	m_d3dOffset;
};

class CSphereVelocitySpawner : public IParticleSpawners
{
public:
	CSphereVelocitySpawner();
	~CSphereVelocitySpawner();
	CSphereVelocitySpawner(float speed);

	virtual void Spawn(float dt, CParticleSystem* sys, unsigned int start, unsigned int end) override;

private:
	float m_fSpeed;
};

class CUniformTimeSpawner : public IParticleSpawners
{
public:
	CUniformTimeSpawner();
	~CUniformTimeSpawner();
	CUniformTimeSpawner(float duration);

	virtual void Spawn(float dt, CParticleSystem* sys, unsigned int start, unsigned int end) override;

private:
	float m_fDuration;
};

#endif//_PARTICLE_SPAWNERS_H_