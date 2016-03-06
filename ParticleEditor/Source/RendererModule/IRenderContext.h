/***********************************************
* Filename:           	IRenderContext.h
* Date:  	         	02/06/2016
* Mod. Date:    	   	02/23/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Interface for the different 
*					render contexts to inherit
************************************************/
#ifndef _IRENDER_CONTEXT_H_
#define _IRENDER_CONTEXT_H_

#include "../Interface.h"
#include "RenderDefines.h"

class CTexture;

CREATE_INTERFACE(IRenderContext)
{
	IRenderContext() {}
	virtual ~IRenderContext() {}

	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, CTexture* defaultTexture) = 0;
	virtual void Render(void) = 0;
	virtual void AddRenderComponent(CObject* obj) = 0;
	virtual void RemoveRenderComponent(CObject* obj) = 0;
	virtual void ClearContainer(void) = 0;

protected:
	ID3D11Device*			m_pd3dDevice;
	ID3D11DeviceContext*	m_pd3dDeviceContext;

	ID3D11VertexShader*		m_pd3dVertexShader;
	ID3D11PixelShader*		m_pd3dPixelShader;
	ID3D11InputLayout*		m_pd3dInputLayout;

	virtual void CreateShadersAndInputLayout(void) = 0;
};

#endif//_IRENDER_CONTEXT_H_