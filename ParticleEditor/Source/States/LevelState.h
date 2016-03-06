/***********************************************
* Filename:           	LevelState.h
* Date:  	         	02/23/2016
* Mod. Date:    	   	02/23/2016
* Mod. Initials:		SH
* Author:	         	Samuel Hotchkiss
* Purpose:            	State that controls gameplay logic
************************************************/

#ifndef _LEVELSTATE_H_
#define _LEVELSTATE_H_

#include "IState.h"

class CLevel;
class CEmitterManager;
class CAssetManager;
class CRenderer;

class CLevelState : public IState
{
	//his own personal level to play with
	CLevel* m_pLevel;

	CEmitterManager* m_pEmitterManager;
	CAssetManager* m_pAssMan;
	CRenderer* m_pRenderer;


public:

	CLevelState();
	CLevelState(std::string);
	~CLevelState();

	eStateType GetStateType() { return eLEVEL; }
	void Enter(CGame*, Camera*);
	void Update(float);
	void Exit();

};

#endif