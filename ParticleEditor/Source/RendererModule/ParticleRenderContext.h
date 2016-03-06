/***********************************************
* Filename:           	ParticleRenderContext.h
* Date:  	         	02/10/2016
* Mod. Date:    	   	02/10/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Define the pipeline for drawing
				particles to the scene. Will also update
				appropriate vertex buffers of relevant data
************************************************/
#ifndef _PARTICLE_RENDER_CONTEXT_H_
#define _PARTICLE_RENDER_CONTEXT_H_

#include "RenderDefines.h"
#include "IRenderContext.h"

class CParticleRenderContext : public IRenderContext
{
public:
	CParticleRenderContext();
	virtual ~CParticleRenderContext();

	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, CTexture* defaultTexture) override;
	virtual void Render(void) override;
	virtual void AddRenderComponent(CObject* obj) override;
	virtual void RemoveRenderComponent(CObject* obj) override;
	virtual void ClearContainer(void) override;

private:
	virtual void CreateShadersAndInputLayout() override;

	ID3D11GeometryShader*			m_pd3dGeometryShader;

	std::vector<CRenderObject*>		m_vpParticles;
};

#endif//_PARTICLE_RENDER_CONTEXT_H_