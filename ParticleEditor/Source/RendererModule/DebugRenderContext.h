/***********************************************
* Filename:           	DebugRenderContext.h
* Date:  	         	02/06/2016
* Mod. Date:    	   	02/06/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Draw transform lines, unit grid,
					collider shapes and anything else
					that can be represented with lines
************************************************/
#ifndef _DEBUG_RENDER_CONTEXT_H_
#define _DEBUG_RENDER_CONTEXT_H_

#include "IRenderContext.h"

class CDebugRenderContext : public IRenderContext
{
public:
	CDebugRenderContext();
	virtual ~CDebugRenderContext();

	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, CTexture* defaultTexture) override;
	virtual void Render(void) override;
	virtual void AddRenderComponent(CObject* obj) override;
	virtual void RemoveRenderComponent(CObject* obj) override;
	virtual void ClearContainer(void) override;

private:
	virtual void CreateShadersAndInputLayout() override;

	std::vector<CRenderObject*>		m_vDebugObjects;
};


#endif//_DEBUG_RENDER_CONTEXT_H_