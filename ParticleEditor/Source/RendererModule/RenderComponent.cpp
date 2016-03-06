/***********************************************
* Filename:           	RenderComponent.cpp
* Date:  	         	02/06/2016
* Mod. Date:    	   	02/15/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Information for Renderer to
*					draw and sort this object
************************************************/
#include "RenderComponent.h"
#include "../Log.h"

CRenderComponent::CRenderComponent()
{
}
CRenderComponent::CRenderComponent(void* mesh, eRenderType type)
{
	m_eRenderType = type;
}
CRenderComponent::~CRenderComponent()
{
}
IComponent::eComponentType CRenderComponent::GetType()
{
	return eRENDER;
}
CRenderComponent::eRenderType CRenderComponent::GetRenderType(void) const
{
	return m_eRenderType;
}
CTexture* CRenderComponent::GetTexture() const
{
	return m_pTexture;
}

std::vector<CTexture*>* CRenderComponent::GetTextures()
{
	return &m_pTextures;
}

CTexture* CRenderComponent::GetTextureInArray(unsigned int _index) const
{
	if (_index >= m_pTextures.size())
	{
		Log("Render component is attempting to return an invalid texture");
		return nullptr;
	}
	else
	{
		return m_pTextures[_index];
	}
}

void CRenderComponent::SetTexture(CTexture* texture)
{
	m_pTexture = texture;
}

void CRenderComponent::SetTextureInArray(CTexture* _texture, unsigned int _index)
{
	if (_index >= m_pTextures.size())
	{
		Log("Render component is attempting to access an invalid texture index");
	}
	else
	{
		m_pTextures[_index] = _texture;
	}
}

void CRenderComponent::SwapToTexture(unsigned int _index)
{
	if (_index >= m_pTextures.size())
	{
		Log("Render component is attempting to swap to a invalid texture index");
	}
	else
	{
		m_pTexture = m_pTextures[_index];
	}
	m_pTexture = m_pTextures[_index];
}
