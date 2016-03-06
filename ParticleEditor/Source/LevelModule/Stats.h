/***********************************************
* Filename:           	Stats.h
* Date :				03 / 03 / 2016
* Mod.Date :			03 / 03 / 2016
* Mod.Initials :		DC
* Author :				Dakota Cardillo
* Purpose :				Attached to objects to
*						give them common properties
************************************************/

#ifndef _STATS_H_
#define _STATS_H_

#include "../LevelModule/IComponent.h"
#include "../SimpleMath.h"

using namespace DirectX::SimpleMath;

class Stats : IComponent
{
public:
	Stats();
	~Stats();

	IComponent::eComponentType GetType() override;
protected:
	

private:

	// Health and stuff
	float m_fHealth;
	float m_fZen;
	float m_fMaxHealth;
	float m_fMaxZen;

	// Location
	Vector3 m_d3dPosition;
	Vector3 m_d3dLastPosition;

	// Speed variables
	float m_fMovementSpeed;
	float m_fAttackSpeed;
};

#endif //_STATS_H_