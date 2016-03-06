//--------------------------------------------------------------------------------
//	CPuppet class
//	Written by: Logan Stallman (LMS)
//	Date:		2/5/2016
//
//	The puppet is a class that can receive input and move around and stuff!
//--------------------------------------------------------------------------------
/* For Windows 7 user- put into configuration properties\C/C++\commandline.
 /D _WIN32_WINNT=0x0601
*/
#ifndef _PUPPET_H_
#define _PUPPET_H_

#include <DirectXMath.h>
#include "CGameObject.h"
#include "../EventModule/EventArgs.h"


class CPuppet : 
	public CGameObject
{
private:
	CPuppet();
	void HackUpdateAnimate(float _dt);


protected:

	virtual bool ResolveHitAI(CObject* _otherThing);

	// Zen increases with good play and builds damage of thrust attack.
	float m_fZenMax;
	float m_fZenCur;

	// maximum squared distance to lock on to targets.
	float m_fLockOnThreshold;


public:
	CPuppet(eObjectTag _tag, CObject* _prefab = nullptr);
	virtual ~CPuppet();

	virtual void Update(float _dt);

	Camera* GetCamera() { return m_pCamera; }
	float GetZenMax() { return m_fZenMax; }
	float GetZenCur() { return m_fZenCur; }

	// Puppet Action functions- return true on success, false on failure
	// call these to make the puppet do the things!

	// mostly the same as CObject but uses the class's movespeed variable by default.
	virtual bool Move(XMFLOAT3 _dir, float _speed = -1.0f);
	
	virtual bool ResolveHit(CObject* _otherThing);

	// Attack functions
	virtual bool AttackVert();
	virtual bool AttackLTR();
	virtual bool AttackRTL();
	virtual bool AttackThrust();
	virtual bool AttackKick();

	// Guard functions
	virtual bool GuardVert();
	virtual bool GuardLTR();
	virtual bool GuardRTL();
	virtual void CancelGuard();

	// Dodge functions
	virtual bool DodgeForward();
	virtual bool DodgeBackward();
	virtual bool DodgeLeft();
	virtual bool DodgeRight();

	// You know it, dood!
	virtual bool Dance();

	// For AI
	virtual bool BecomeVulnerable(float _duration = -1.0f);

	CPuppet* SendSelf(void* empty = nullptr);

	bool m_bHackPuppetPause;

	unsigned int m_uDefHP;
	unsigned int m_uCurHP;
};

#endif