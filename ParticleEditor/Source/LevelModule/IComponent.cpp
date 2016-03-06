/***********************************************
* Filename:           	IComponent.cpp
* Date :				02 / 06 / 2016
* Mod.Date :			02 / 06 / 2016
* Mod.Initials :		JS
* Author :				Joshua Stankiewicz
* Purpose :				Interface for components
*               	to inherit from
************************************************/
#include "IComponent.h"

IComponent::IComponent() : m_pobjParent(nullptr)
{
}
IComponent::~IComponent()
{
}
CObject* IComponent::GetParent(void) const
{
	return m_pobjParent;
}
void IComponent::SetParent(CObject* obj)
{
	m_pobjParent = obj;
}