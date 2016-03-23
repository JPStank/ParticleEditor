/***********************************************
* Filename:           	ParticleSpawners.h
* Date :				02 / 09 / 2016
* Mod.Date :			03 / 08 / 2016
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

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) = 0;
};

class CBoxSpawner : public IParticleSpawners
{
public:
	CBoxSpawner();
	~CBoxSpawner();
	CBoxSpawner(float3 center, float3 offset);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float3	m_d3dCenter;
	float3	m_d3dOffset;
};

class CCircleSpawner : public IParticleSpawners
{
public:
	CCircleSpawner();
	~CCircleSpawner();
	CCircleSpawner(float3 center, float radius, float speed, std::string axis);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float3		m_d3dCenter;
	float		m_fRadius;
	float		m_fSpeed;
	std::string	m_szAxis;
};

class CCircleRandSpeedSpawner : public IParticleSpawners
{
public:
	CCircleRandSpeedSpawner();
	~CCircleRandSpeedSpawner();
	CCircleRandSpeedSpawner(float3 center, float radius, float minSpeed, float maxSpeed, std::string axis);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float3		m_d3dCenter;
	float		m_fRadius;
	float		m_fMinSpeed;
	float		m_fMaxSpeed;
	std::string	m_szAxis;
};

class CConeRandSpeedSpawner : public IParticleSpawners
{
public:
	CConeRandSpeedSpawner();
	~CConeRandSpeedSpawner();
	CConeRandSpeedSpawner(float3 center, float halfAngle, float minSpeed, float maxSpeed);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float3		m_d3dCenter;
	float		m_fHalfAngle;
	float		m_fMinSpeed;
	float		m_fMaxSpeed;
};
class CConeSpawner : public IParticleSpawners
{
public:
	CConeSpawner();
	~CConeSpawner();
	CConeSpawner(float3 center, float halfAngle, float speed);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float3		m_d3dCenter;
	float		m_fHalfAngle;
	float		m_fSpeed;
};

class CSphereVelocitySpawner : public IParticleSpawners
{
public:
	CSphereVelocitySpawner();
	~CSphereVelocitySpawner();
	CSphereVelocitySpawner(float speed);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float m_fSpeed;
};
class CSphereRandomVelocitySpawner : public IParticleSpawners
{
public:
	CSphereRandomVelocitySpawner();
	~CSphereRandomVelocitySpawner();
	CSphereRandomVelocitySpawner(float minSpeed, float maxSpeed);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float m_fMinSpeed;
	float m_fMaxSpeed;
};

class CUniformTimeSpawner : public IParticleSpawners
{
public:
	CUniformTimeSpawner();
	~CUniformTimeSpawner();
	CUniformTimeSpawner(float duration);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float m_fDuration;
};

class CRandomTimeSpawner : public IParticleSpawners
{
public:
	CRandomTimeSpawner();
	~CRandomTimeSpawner();
	CRandomTimeSpawner(float minDuration, float maxDuration);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float m_fMinDuration;
	float m_fMaxDuration;
};

class CRandomScaleSpawner : public IParticleSpawners
{
public:
	CRandomScaleSpawner();
	~CRandomScaleSpawner();
	CRandomScaleSpawner(float minScale, float maxScale);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float m_fMinScale;
	float m_fMaxScale;
};

class CUniformScaleSpawner : public IParticleSpawners
{
public:
	CUniformScaleSpawner();
	~CUniformScaleSpawner();
	CUniformScaleSpawner(float scale);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float m_fScale;
};

class CUniformRotationSpawner : public IParticleSpawners
{
public:
	CUniformRotationSpawner();
	~CUniformRotationSpawner();
	CUniformRotationSpawner(float degrees);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float m_fRotation;
};
class CRandomRotationSpawner : public IParticleSpawners
{
public:
	CRandomRotationSpawner();
	~CRandomRotationSpawner();
	CRandomRotationSpawner(float minRot, float maxRot);

	virtual void Spawn(CParticleSystem* sys, unsigned int start, unsigned int end) override;

public:
	float m_fMinRotation;
	float m_fMaxRotation;
};

#endif//_PARTICLE_SPAWNERS_H_