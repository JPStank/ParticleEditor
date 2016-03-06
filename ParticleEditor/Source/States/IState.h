/***********************************************
* Filename:           	IState.h
* Date:  	         	02/23/2016
* Mod. Date:    	   	02/23/2016
* Mod. Initials:		SH
* Author:	         	Samuel Hotchkiss
* Purpose:            	State system to assist with different kinds of game logic
************************************************/

#ifndef _ISTATE_H_
#define _ISTATE_H_

#include <string>

class CGame;
class Camera;

class IState
{
protected:
	//IState *prevState, *nextState;
	std::string m_szLevelToLoad = "";

public:

	enum eStateType {eLEVEL = 0, eMAINMENU};

	IState();
	virtual ~IState();

	virtual eStateType GetStateType() = 0;
	virtual void Enter(CGame*, Camera*) = 0;
	virtual void Update(float) = 0;
	virtual void Exit() = 0;
};

#endif