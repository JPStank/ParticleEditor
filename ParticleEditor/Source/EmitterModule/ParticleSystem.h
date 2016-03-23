/***********************************************
* Filename:           	ParticleSystem.h
* Date :				02 / 09 / 2016
* Mod.Date :			03 / 08 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Component that holds onto
					the raw data of particles needing
					to be rendered and emitter properties
************************************************/
#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#include "../LevelModule/IComponent.h"
#include "ParticleDefines.h"

struct IParticleSpawners;
struct IParticleUpdater;

class CParticleSystem : public IComponent
{
public:
	CParticleSystem();
	CParticleSystem(unsigned int maxCount, float rate);
	~CParticleSystem();

	virtual eComponentType GetType();

	void Kill(unsigned int id);
	void Wake(unsigned int id);
	void SwapData(unsigned int a, unsigned int b);

	std::vector<DirectX::XMFLOAT4>& GetPosition();
	std::vector<DirectX::XMFLOAT4>& GetColor();
	std::vector<DirectX::XMFLOAT4>& GetVelocity();
	std::vector<DirectX::XMFLOAT4>& GetTime();
	std::vector<float>& GetScale();
	std::vector<float>& GetRotation();

	unsigned int GetAlive() const;
	unsigned int GetMax() const;
	float GetRate() const;

	void Update(float dt);

	std::vector<IParticleSpawners*>& GetSpawners();
	std::vector<IParticleUpdater*>& GetUpdaters();

	// normally set to private, exposing for ease of editor attachment
public:
	std::vector<DirectX::XMFLOAT4>		m_vd3dPositions;
	std::vector<DirectX::XMFLOAT4>		m_vd3dColor;
	std::vector<DirectX::XMFLOAT4>		m_vd3dVelocity;
	std::vector<DirectX::XMFLOAT4>		m_vd3dTime;
	std::vector<float>					m_vfScale;
	std::vector<float>					m_vfRotation;
	std::vector<bool>					m_vbAlive;

	unsigned int		m_unMaxCount;
	unsigned int		m_unAliveCount;
	float				m_fEmitRate;
	float				m_fTimer;

	std::vector<IParticleSpawners*>		m_vpSpawners;
	std::vector<IParticleUpdater*>		m_vpUpdaters;
};

#endif//_PARTICLE_SYSTEM_H_