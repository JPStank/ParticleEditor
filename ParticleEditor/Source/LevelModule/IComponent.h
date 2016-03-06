/***********************************************
* Filename:           	IComponent.h
* Date :				02 / 06 / 2016
* Mod.Date :			02 / 06 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Interface for components
*               	to inherit from
************************************************/
#ifndef _ICOMPONENT_H_
#define _ICOMPONENT_H_

#include "../Interface.h"

class CObject;

CREATE_INTERFACE(IComponent)
{
	// TODO: add anything I've missed - JS
	enum eComponentType { eEMPTY = 0, eRENDER, eLIGHT, eEMITTER, eCOLLIDER, eRIGID_BODY, eANIMATION, eAGENT, eUI, ePLAYER, eMENU, eLEVELTRANSIT };

	IComponent();
	virtual ~IComponent();
	virtual eComponentType GetType() = 0; // all components must overwrite this function to return their type. should not be stored in member variable - JS
	virtual bool GetActive() { return m_bActive; }
	CObject* GetParent(void) const;

	void SetActive(bool _active) { m_bActive = _active; }
	void SetParent(CObject* obj);

protected:
	CObject* m_pobjParent;
	bool m_bActive = true;
};

#endif//_ICOMPONENT_H_