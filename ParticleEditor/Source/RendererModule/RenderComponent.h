/***********************************************
* Filename:           	RenderComponent.h
* Date:  	         	02/06/2016
* Mod. Date:    	   	02/06/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Information for Renderer to
*					draw and sort this object
************************************************/
#ifndef _RENDER_COMPONENT_H_
#define _RENDER_COMPONENT_H_

#include "../LevelModule/IComponent.h"

#include <vector>

class CTexture;

class CRenderComponent : public IComponent
{
public:
	enum eRenderType { eDEBUG = 0, eSTATIC, ePARTICLE, eANIMATED, eHUD };

	CRenderComponent();
	CRenderComponent(void* mesh, eRenderType type);
	virtual ~CRenderComponent();

	eComponentType GetType() override;
	eRenderType GetRenderType(void) const;

	CTexture* GetTexture() const;
	std::vector<CTexture*>* GetTextures();
	CTexture* GetTextureInArray(unsigned int _index) const;
	void SetTexture(CTexture* texture);
	void SetTextureInArray(CTexture* _texture, unsigned int _index);
	void AddToTextureArray(CTexture* _texture) { m_pTextures.push_back(_texture); }

	void SwapToTexture(unsigned int _index);

private:
	CTexture*		m_pTexture;
	std::vector<CTexture*>		m_pTextures;
	eRenderType		m_eRenderType;
};


#endif//_RENDER_COMPONENT_H_