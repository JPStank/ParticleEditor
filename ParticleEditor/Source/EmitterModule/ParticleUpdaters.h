/***********************************************
* Filename:           	ParticleUpdaters.h
* Date :				02 / 09 / 2016
* Mod.Date :			03 / 08 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Define the different updates
that a given system will apply to particles
************************************************/
#ifndef _PARTICLE_UPDATERS_H_
#define _PARTICLE_UPDATERS_H_

#include "ParticleDefines.h"
#include "../Interface.h"

class CParticleSystem;

CREATE_INTERFACE(IParticleUpdater)
{
	IParticleUpdater(){}
	virtual ~IParticleUpdater(){}

	virtual void Update(float dt, CParticleSystem* sys) = 0;
};

class CEulerUpdater : public IParticleUpdater
{
public:
	CEulerUpdater();
	CEulerUpdater(float3 acceleration);
	virtual ~CEulerUpdater();

	virtual void Update(float dt, CParticleSystem* sys) override;

private:
	float3	m_d3dAcceleration;
};

class CColorUpdater : public IParticleUpdater
{
public:
	CColorUpdater();
	CColorUpdater(float4 startColor, float4 endColor);
	virtual ~CColorUpdater();

	virtual void Update(float dt, CParticleSystem* sys) override;

private:
	float4 m_d3dStartColor;
	float4 m_d3dEndColor;
};

class CScaleUpdater : public IParticleUpdater
{
public:
	CScaleUpdater();
	CScaleUpdater(float startScale, float endScale);
	virtual ~CScaleUpdater();

	virtual void Update(float dt, CParticleSystem* sys) override;

private:
	float m_fStartScale;
	float m_fEndScale;
};

class CRotationUpdater : public IParticleUpdater
{
public:
	CRotationUpdater();
	CRotationUpdater(float anglesPerSecond);
	virtual ~CRotationUpdater();

	virtual void Update(float dt, CParticleSystem* sys) override;

private:
	float m_fSpeed;
};

class CTimeUpdater : public IParticleUpdater
{
public:
	CTimeUpdater();
	virtual ~CTimeUpdater();

	virtual void Update(float dt, CParticleSystem* sys) override;
};

#endif//_PARTICLE_UPDATERS_H_