/***********************************************
* Filename:           	Renderer.h
* Date:  	         	02/07/2016
* Mod. Date:    	   	02/23/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Rendering powerhouse
************************************************/
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <directxmath.h>
#include <stdint.h>
#include <vector>
#include <map>

#include "RenderDefines.h"

struct IComponent;
struct IRenderContext;
class CTexture;
class Camera;
class CDebugRenderContext;
class CParticleRenderContext;

class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	// create device and swap chain. also all other needed components to run the renderer
	bool Initialize(int width, int height, HWND hwnd);

	// free all of the data
	bool ShutDown(void);

	// placeholder for now, will implemenet if derive from iBaseModule
	void Update(float dt);

	// draw all of the things
	void Render(void);

	// let the renderer know about what has to be drawn
	void RegisterComponent(IComponent* comp);

	// let renderer know about things no longer needing to be drawn
	void UnregisterComponent(IComponent* comp);

	// use this to tell the renderer which camera to use
	void AddCamera(Camera* cam);

	// clear all render contexts for state changes
	void ClearContexts(void);

	// window resize nonsense
	bool GetWindowed(void) const;
	void SetWindowed(int width, int height, bool windowed);
	void ResizeBuffers(int width, int height);
	void ResizeTarget(int width, int height, bool windowed);
	void SetSwapChainState(bool fullscreen);
	void CreateBuffers(const DXGI_MODE_DESC& desc);
	void ReleaseBuffers(void);
	bool GetModeDesc(DXGI_MODE_DESC& config);
	void SetModeDesc(DXGI_MODE_DESC& config);
	bool FindAdapter(IDXGIFactory1** factory, IDXGIAdapter1** adapter);

	// asset manager may need these for creating items
	ID3D11Device* GetDevice(void) const;
	ID3D11DeviceContext* GetDeviceContext(void) const;

private:
	void SetupSamplerStates(void);
	void TurnDepthBufferOn(void);
	void TurnDepthBufferOff(void);
	void TurnAlphaBlendingOn(void);
	void TurnAlphaBlendingOff(void);

	IDXGISwapChain*				m_pd3dSwapChain;
	ID3D11Texture2D*			m_pd3dBackBuffer;
	ID3D11Device*				m_pd3dDevice;
	ID3D11DeviceContext*		m_pd3dDeviceContext;
	ID3D11RenderTargetView*		m_pd3dRenderTargetView;
	ID3D11Texture2D*			m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilState*	m_pd3dDepthStencilStateEnabled;
	ID3D11DepthStencilState*	m_pd3dDepthStencilStateDisabled;
	ID3D11DepthStencilView*		m_pd3dDepthStencilView;
	ID3D11RasterizerState*		m_pd3dRasterState;

	ID3D11BlendState*			m_pd3dAlphaEnableBlendingState;
	ID3D11BlendState*			m_pd3dAlphaDisableBlendingState;

	TPerFrame					m_tSceneMatricies;
	ID3D11Buffer*				m_pd3dSceneBuffer;
	Camera*						m_pCamera;

	ID3D11SamplerState*			m_pd3dSampleStateWrap;
	ID3D11SamplerState*			m_pd3dSampleStateClamp;
	ID3D11ShaderResourceView*	m_pd3dTexture; // default texture
	ID3D11ShaderResourceView*	m_pd3dTestTexture; // for quick testing assets
	CTexture*					m_pDefaultTexture;
	CTexture*					m_pTestTexture;

	CDebugRenderContext*		m_pDebugRC;
	CParticleRenderContext*		m_pParticleRC;
};

#endif // _RENDERER_H_
