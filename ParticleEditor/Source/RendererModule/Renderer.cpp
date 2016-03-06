/***********************************************
* Filename:           	Renderer.cpp
* Date:  	         	02/07/2016
* Mod. Date:    	   	02/15/2016
* Mod. Initials:		JS
* Author:	         	Joshua Stankiewicz
* Purpose:				Rendering powerhouse
************************************************/
#include "./Renderer.h"
#include "../DXLog.h"
#include "../Texture2d.h"
#include "../ExporterHeader.h"
#include "../EventModule/EventManager.h"

#include "../AssetModule/Texture.h"
#include "../AssetModule/DDSTextureLoader.h"
#include "../LevelModule/Object.h"
#include "../LevelModule/Camera.h"
#include "RenderComponent.h"
#include "DebugRenderContext.h"
#include "ParticleRenderContext.h"

#include <fstream>

//--------------------------------------------------------------------------------
//include the needed libraries for rendering
#pragma comment(lib,"d3d11.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")

//--------------------------------------------------------------------------------

bool g_windowed = true;
IDXGIOutput *			g_pVideoOutput = nullptr;
std::vector<DXGI_MODE_DESC> gDescs;

CRenderer::CRenderer()
{
	m_pd3dSwapChain = nullptr;
	m_pd3dBackBuffer = nullptr;
	m_pd3dDevice = nullptr;
	m_pd3dDeviceContext = nullptr;
	m_pd3dRenderTargetView = nullptr;
	m_pd3dDepthStencilBuffer = nullptr;
	m_pd3dDepthStencilStateEnabled = nullptr;
	m_pd3dDepthStencilStateDisabled = nullptr;
	m_pd3dDepthStencilView = nullptr;
	m_pd3dRasterState = nullptr;

	m_pd3dAlphaDisableBlendingState = nullptr;
	m_pd3dAlphaEnableBlendingState = nullptr;

	ZeroMemory(&m_tSceneMatricies, sizeof(m_tSceneMatricies));
	m_pd3dSceneBuffer = nullptr;
	m_pCamera = nullptr;

	m_pd3dSampleStateWrap = nullptr;
	m_pd3dSampleStateClamp = nullptr;
	m_pd3dTexture = nullptr;
	
	m_pDebugRC = nullptr;

	m_pDefaultTexture = nullptr;
}
CRenderer::~CRenderer()
{

}
bool CRenderer::Initialize(int width, int height, HWND hwnd)
{
	HRESULT result;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	//ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;


	IDXGIFactory1* factory = nullptr;
	IDXGIAdapter1* adapter = nullptr;


	if (!FindAdapter(&factory, &adapter))
	{
		Log("Unable to find adapter!");
		return false;
	}

	DXCall(result = adapter->EnumOutputs(0, &g_pVideoOutput));

	// set up swapchain, device and context
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.Windowed = (BOOL)g_windowed;// (fullscreen ? FALSE : TRUE);

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.Flags = 0;

	// used in lieu of default full list
	// will force error/crash if d3d11 is not supported
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT flags = 0;

#if _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXCall(result = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_pd3dSwapChain,
		&m_pd3dDevice,
		NULL,
		&m_pd3dDeviceContext));

	if (SUCCEEDED(result))
	{
		Log("Device and swap chain created.");
	}
	else
	{
		Log("Renderer did a bad. No device 4 U");
		return false;
	}

	DXNameInit(m_pd3dDeviceContext);
	DXName(m_pd3dDevice, "The Device");
	DXName(m_pd3dDeviceContext, "The Device Context");
	DXName(m_pd3dSwapChain, "The Swap Chain");

	// attach backbuffer to RTV
	DXCall(result = m_pd3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_pd3dBackBuffer));
	DXCall(result = m_pd3dDevice->CreateRenderTargetView(m_pd3dBackBuffer, NULL, &m_pd3dRenderTargetView));
	//SAFE_RELEASE(backBufferPtr);

	DXName(m_pd3dRenderTargetView, "Backbuffer RTV");

	// setup depth buffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	DXCall(result = m_pd3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pd3dDepthStencilBuffer));
	DXName(m_pd3dDepthStencilBuffer, "Z Buffer");

	// set up depth stencil state
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xff;
	depthStencilDesc.StencilWriteMask = 0xff;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	DXCall(result = m_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_pd3dDepthStencilStateEnabled));
	DXName(m_pd3dDepthStencilStateEnabled, "Depth Enabled State");

	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilStateEnabled, 1);


	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	DXCall(result = m_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_pd3dDepthStencilStateDisabled));
	DXName(m_pd3dDepthStencilStateDisabled, "Depth Disabled State");


	// set up depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	DXCall(result = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &depthStencilViewDesc, &m_pd3dDepthStencilView));
	DXName(m_pd3dDepthStencilView, "Depth Stencil View");

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

	// set up rasterizer state
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = TRUE;
	rasterDesc.MultisampleEnable = FALSE;
	rasterDesc.ScissorEnable = FALSE;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	DXCall(result = m_pd3dDevice->CreateRasterizerState(&rasterDesc, &m_pd3dRasterState));
	DXName(m_pd3dRasterState, "Backface Culled Raster State");

	m_pd3dDeviceContext->RSSetState(m_pd3dRasterState);

	// setup viewport
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = viewport.TopLeftY = 0.0f;

	m_pd3dDeviceContext->RSSetViewports(1, &viewport);

	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));
	cb.ByteWidth = sizeof(TPerFrame);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DXCall(result = m_pd3dDevice->CreateBuffer(&cb, NULL, &m_pd3dSceneBuffer));
	DXName(m_pd3dSceneBuffer, "View Proj cBuffer");

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	// enabled blending
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	DXCall(result = m_pd3dDevice->CreateBlendState(&blendDesc, &m_pd3dAlphaEnableBlendingState));
	DXName(m_pd3dAlphaEnableBlendingState, "Alpha Blending Enabled");

	// disabled blending
	blendDesc.RenderTarget[0].BlendEnable = FALSE;

	DXCall(result = m_pd3dDevice->CreateBlendState(&blendDesc, &m_pd3dAlphaDisableBlendingState));
	DXName(m_pd3dAlphaDisableBlendingState, "Alpha Blending Disabled");


	SetupSamplerStates();

	//init render contexts
	m_pDebugRC = new CDebugRenderContext();
	m_pDebugRC->Initialize(m_pd3dDevice, m_pd3dDeviceContext, m_pDefaultTexture);

	m_pParticleRC = new CParticleRenderContext();
	m_pParticleRC->Initialize(m_pd3dDevice, m_pd3dDeviceContext, m_pDefaultTexture);


	return true;
}
bool CRenderer::ShutDown(void)
{
	delete m_pDebugRC;
	m_pDebugRC = nullptr;

	delete m_pParticleRC;
	m_pParticleRC = nullptr;

	// bad time if you dont ensure windowed when shutting down
	if (m_pd3dSwapChain)
	{
		m_pd3dSwapChain->SetFullscreenState(FALSE, NULL);
	}

	// bye bye everything
	SAFE_RELEASE(m_pd3dAlphaDisableBlendingState);
	SAFE_RELEASE(m_pd3dAlphaEnableBlendingState);
	SAFE_RELEASE(m_pd3dSwapChain);
	SAFE_RELEASE(m_pd3dRasterState);
	SAFE_RELEASE(m_pd3dDepthStencilView);
	SAFE_RELEASE(m_pd3dDepthStencilStateEnabled);
	SAFE_RELEASE(m_pd3dDepthStencilStateDisabled);
	SAFE_RELEASE(m_pd3dDepthStencilBuffer);
	SAFE_RELEASE(m_pd3dRenderTargetView);
	SAFE_RELEASE(m_pd3dSceneBuffer);
	SAFE_RELEASE(m_pd3dDeviceContext);
	SAFE_RELEASE(m_pd3dDeviceContext);
	SAFE_RELEASE(m_pd3dBackBuffer);

	SAFE_RELEASE(m_pd3dSampleStateClamp);
	SAFE_RELEASE(m_pd3dSampleStateWrap);

	delete m_pDefaultTexture;
	m_pDefaultTexture = nullptr;

	delete m_pTestTexture;
	m_pTestTexture = nullptr;

	//ID3D11Debug* debug;
	//DXCall(m_pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug)));
	//debug->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
	
	//SAFE_RELEASE(debug);
	//SAFE_RELEASE(m_pd3dDevice); // WHY?!

	DXDebugRelease();

	return true;
}
void CRenderer::Update(float dt)
{
	Render();
}
void CRenderer::Render(void)
{
	float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, color);
	m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_tSceneMatricies.m_d3dViewMatrix = m_pCamera->GetView();
	m_tSceneMatricies.m_d3dProjMatrix = m_pCamera->GetProjection();

	D3D11_MAPPED_SUBRESOURCE data;
	m_pd3dDeviceContext->Map(m_pd3dSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &data);
	memcpy(data.pData, &m_tSceneMatricies, sizeof(TPerFrame));
	m_pd3dDeviceContext->Unmap(m_pd3dSceneBuffer, 0);

	m_pd3dDeviceContext->VSSetConstantBuffers(1, 1, &m_pd3dSceneBuffer);
	m_pd3dDeviceContext->GSSetConstantBuffers(1, 1, &m_pd3dSceneBuffer);
	//m_pd3dDeviceContext->PSSetConstantBuffers(1, 1, &m_pd3dSceneBuffer);

	// render each context

	m_pDebugRC->Render();

	TurnAlphaBlendingOn();
	m_pParticleRC->Render();
	TurnAlphaBlendingOff();

	m_pd3dSwapChain->Present(0, 0);
}
void CRenderer::RegisterComponent(IComponent* comp)
{
	if (comp == nullptr)
	{
		Log("Please don't pass null into the Renderer");
		return;
	}
	if (((CRenderComponent*)comp)->GetRenderType() != CRenderComponent::eRenderType::ePARTICLE &&
		((CRenderComponent*)comp)->GetRenderType() != CRenderComponent::eRenderType::eHUD &&
		comp->GetType() != IComponent::eLIGHT)
	{
		Log("Unknown configuration given to ", __FUNCTION__); // new render types without a mesh might need checking
		return;
	}
	if (comp->GetType() == IComponent::eRENDER ||
		comp->GetType() == IComponent::eLIGHT)
	{

		if (comp->GetType() == IComponent::eRENDER)
		{
			switch (((CRenderComponent*)comp)->GetRenderType())
			{
			case CRenderComponent::eRenderType::ePARTICLE:
			{
				m_pParticleRC->AddRenderComponent(comp->GetParent());
				break;
			}
			case CRenderComponent::eRenderType::eHUD:
			case CRenderComponent::eRenderType::eANIMATED:
			case CRenderComponent::eRenderType::eDEBUG:
			case CRenderComponent::eRenderType::eSTATIC:
			default:
			{
				Log("Render type that was not Particle given to renderer.");
				break;
			}
			}
#if _DEBUG
			if (((CRenderComponent*)comp)->GetRenderType() != CRenderComponent::eRenderType::eHUD)
			{
				m_pDebugRC->AddRenderComponent(comp->GetParent());
			}
#endif
		}
	}
	else
	{
		Log("Renderer was given something that wasn't render or light component");
	}
}
void CRenderer::UnregisterComponent(IComponent* comp)
{
	if (comp == nullptr)
	{
		Log("Please don't pass null into the Renderer");
		return;
	}

	if (comp->GetType() == IComponent::eRENDER ||
		comp->GetType() == IComponent::eLIGHT)
	{

		if (comp->GetType() == IComponent::eRENDER)
		{
			switch (((CRenderComponent*)comp)->GetRenderType())
			{
			case CRenderComponent::eRenderType::ePARTICLE:
			{
				m_pParticleRC->RemoveRenderComponent(comp->GetParent());
				break;
			}
			case CRenderComponent::eRenderType::eHUD:
			case CRenderComponent::eRenderType::eANIMATED:
			case CRenderComponent::eRenderType::eDEBUG:
			case CRenderComponent::eRenderType::eSTATIC:
			default:
			{
				Log("Render type that was not Particle given to renderer.");
				break;
			}
			}
#if _DEBUG
			if (((CRenderComponent*)comp)->GetRenderType() != CRenderComponent::eRenderType::eHUD)
			{
				m_pDebugRC->RemoveRenderComponent(comp->GetParent());
			}
#endif
		}
	}
	else
	{
		Log("Renderer was given something that wasn't render or light component");
	}
}
ID3D11Device* CRenderer::GetDevice(void) const
{
	return m_pd3dDevice;
}
ID3D11DeviceContext* CRenderer::GetDeviceContext(void) const
{
	return m_pd3dDeviceContext;
}
void CRenderer::SetupSamplerStates(void)
{
	HRESULT result;
	D3D11_SAMPLER_DESC samp;
	ZeroMemory(&samp, sizeof(samp));

	samp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp.MinLOD = -FLT_MAX;
	samp.MaxLOD = FLT_MAX;
	samp.MaxAnisotropy = 1;
	samp.ComparisonFunc = D3D11_COMPARISON_NEVER;

	DXCall(result = m_pd3dDevice->CreateSamplerState(&samp, &m_pd3dSampleStateClamp));
	DXName(m_pd3dSampleStateClamp, "Clamp Sampler");

	samp.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samp.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	DXCall(result = m_pd3dDevice->CreateSamplerState(&samp, &m_pd3dSampleStateWrap));
	DXName(m_pd3dSampleStateWrap, "Wrap Sampler");

	ID3D11SamplerState* states[2] = { m_pd3dSampleStateClamp, m_pd3dSampleStateWrap };
	m_pd3dDeviceContext->PSSetSamplers(0, 2, states);

	ID3D11Resource* temp;
	result = DirectX::CreateDDSTextureFromFile(m_pd3dDevice, L"Resources/Texture/white.dds", &temp, &m_pd3dTexture);
	SAFE_RELEASE(temp);

	m_pDefaultTexture = new CTexture();
	m_pDefaultTexture->AssignTexture(CTexture::eDIFFUSE, m_pd3dTexture);

	DXName(m_pd3dTexture, "Default Texture");

	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dTexture);

}
void CRenderer::TurnDepthBufferOn(void)
{
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilStateEnabled, 1);
}
void CRenderer::TurnDepthBufferOff(void)
{
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilStateDisabled, 1);

}
void CRenderer::TurnAlphaBlendingOn(void)
{
	float blend[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	m_pd3dDeviceContext->OMSetBlendState(m_pd3dAlphaEnableBlendingState, blend, 0xffffffff);
}
void CRenderer::TurnAlphaBlendingOff(void)
{
	float blend[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	m_pd3dDeviceContext->OMSetBlendState(m_pd3dAlphaDisableBlendingState, blend, 0xffffffff);
}
void CRenderer::AddCamera(Camera* cam)
{
	m_pCamera = cam;
}
bool CRenderer::GetWindowed(void) const
{
	return g_windowed;
}
void CRenderer::SetWindowed(int width, int height, bool windowed)
{
	ResizeTarget(width, height, windowed);
	SetSwapChainState(!windowed);
}
void CRenderer::ResizeBuffers(int width, int height)
{
	if (nullptr == m_pd3dSwapChain) {
		return;
	}
	HRESULT hr = E_FAIL;
	DXGI_SWAP_CHAIN_DESC config;
	using namespace Events;

	m_pd3dSwapChain->GetDesc(&config);

	if (!(config.BufferDesc.Width == width && config.BufferDesc.Height == height)) {
		EventManager()->SendEvent("ChangeResolution", new  CGeneralEventArgs2<float, float>(static_cast<float>(width), static_cast<float>(height)), CEventManager::eEventInstant);
	}

	config.BufferDesc.Width = width;
	config.BufferDesc.Height = height;
	config.BufferDesc.RefreshRate.Denominator = 0;
	config.BufferDesc.RefreshRate.Numerator = 0;
	config.Windowed = g_windowed;
	GetModeDesc(config.BufferDesc);

	Log("ResizeBuffers ", g_windowed == true ? "Windowed " : "Full Screen ", "Width: ", config.BufferDesc.Width, " Height: ", config.BufferDesc.Height);
	ReleaseBuffers();

	if (FAILED(hr = m_pd3dSwapChain->ResizeBuffers(config.BufferCount, config.BufferDesc.Width, config.BufferDesc.Height, config.BufferDesc.Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH))) {
		DXMemoryCheck(2, m_pd3dDeviceContext);
	}

	CreateBuffers(config.BufferDesc);

}
void CRenderer::ResizeTarget(int width, int height, bool windowed)
{
	DXGI_SWAP_CHAIN_DESC config;
	using namespace Events;
	m_pd3dSwapChain->GetDesc(&config);

	if (!(config.BufferDesc.Width == width &&
		config.BufferDesc.Height == height)) {
		EventManager()->SendEvent("ChangeResolution", new  CGeneralEventArgs<float>(width / static_cast<float>(height)), CEventManager::eEventInstant);
	}

	windowed = config.Windowed = g_windowed;
	config.BufferDesc.Width = width;
	config.BufferDesc.Height = height;
	//Log("Resize Target ", _windowed == false ? "Full Screen" : "Windowed", " w: ", config.BufferDesc.Width, " h: ", config.BufferDesc.Height);
	config.BufferDesc.RefreshRate.Denominator = 0;
	config.BufferDesc.RefreshRate.Numerator = 0;
	GetModeDesc(config.BufferDesc);

	DXCall(m_pd3dSwapChain->ResizeTarget(&config.BufferDesc));
}
void CRenderer::SetSwapChainState(bool fullscreen)
{
	if (nullptr == m_pd3dSwapChain)
	{
		return;
	}
	g_windowed = !fullscreen;
	DXCall(m_pd3dSwapChain->SetFullscreenState(fullscreen, (fullscreen == true ? g_pVideoOutput : nullptr)));
}
void CRenderer::CreateBuffers(const DXGI_MODE_DESC& desc)
{
	//ID3D11Texture2D * pBackBuffer = nullptr;
	DXCall(m_pd3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&m_pd3dBackBuffer));

	DXCall(m_pd3dDevice->CreateRenderTargetView(m_pd3dBackBuffer, NULL, &m_pd3dRenderTargetView));

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = desc.Width;
	depthBufferDesc.Height = desc.Height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	DXCall(m_pd3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pd3dDepthStencilBuffer));
	DXName(m_pd3dDepthStencilBuffer, "Z Buffer");
	
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	DXCall(m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &depthStencilViewDesc, &m_pd3dDepthStencilView));
	DXName(m_pd3dDepthStencilView, "Depth Stencil View");

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
}
void CRenderer::ReleaseBuffers(void)
{
	SAFE_RELEASE(m_pd3dBackBuffer);
	SAFE_RELEASE(m_pd3dDepthStencilBuffer);

	SAFE_RELEASE(m_pd3dRenderTargetView);
	SAFE_RELEASE(m_pd3dDepthStencilView);
}
bool CRenderer::GetModeDesc(DXGI_MODE_DESC& config)
{
	UINT num = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	UINT flags = DXGI_ENUM_MODES_INTERLACED;

	config.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	if (!g_pVideoOutput) {
		Log("No Video Output");
		return false;
	}

	DXCall(g_pVideoOutput->GetDisplayModeList(format, flags, &num, 0));
	if (0 < num) {
		gDescs.resize(num);
		DXCall(g_pVideoOutput->GetDisplayModeList(format, flags, &num, &gDescs[0]));
		int poop = 0;
		bool found = false;
		for (unsigned int i = 0; i < num; ++i) {
			if (config.Format == gDescs[i].Format &&
				config.Height == gDescs[i].Height &&
				config.Width == gDescs[i].Width &&
				config.Scaling == gDescs[i].Scaling) {
				found = true;
				config = gDescs[i];
			}
		}

		if (false == found) {
			Log(__FUNCTION__, " No formats found, looking for close match");
			DXGI_MODE_DESC startDesc = config;
			DXCall(g_pVideoOutput->FindClosestMatchingMode(&startDesc, &config, m_pd3dDevice));
		}
		return true;
	}

	Log(__FUNCTION__, " no list of displays for the video output");
	return false;

}
void CRenderer::SetModeDesc(DXGI_MODE_DESC &_config) {
	if (gDescs[gDescs.size() - 1].Width != _config.Width)
		_config.Scaling = DXGI_MODE_SCALING_STRETCHED;
	else
		_config.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_MODE_DESC startDesc = _config;
	DXCall(g_pVideoOutput->FindClosestMatchingMode(&startDesc, &_config, m_pd3dDevice));
}
bool CRenderer::FindAdapter(IDXGIFactory1** factory, IDXGIAdapter1** adapter)
{
	HRESULT hr = S_OK;
	IDXGIAdapter1 * pCurrentAdapter = nullptr;
	DXGI_ADAPTER_DESC1 Bestdesc;
	if (!factory) {
		Log("Cannot send in a NULL factory");
		return false;
	}
	if (!adapter) {
		Log("Cannot send in a NULL adapter");
		return false;
	}

	// Create a factory to enumerate all of the hardware in the system.
	DXCall(hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory)));

#pragma warning(suppress: 6102)
	if (!*factory) {
		Log("Could not create the factory");
		return false;
	}
	unsigned int index = 0;
	while (DXGI_ERROR_NOT_FOUND != (*factory)->EnumAdapters1(index++, &pCurrentAdapter)) {
		DXGI_ADAPTER_DESC1 desc;
		ZeroMemory(&desc, sizeof(desc));
		DXCall(pCurrentAdapter->GetDesc1(&desc));
		if (nullptr == *adapter) {
			*adapter = pCurrentAdapter;
			Bestdesc = desc;
		}
		else if (Bestdesc.DedicatedSystemMemory < desc.DedicatedSystemMemory) {
			(*adapter)->Release();
			*adapter = pCurrentAdapter;
			Bestdesc = desc;
		}
		else {
			pCurrentAdapter->Release();
		}
	}
	return true;
}
void CRenderer::ClearContexts(void)
{
	m_pDebugRC->ClearContainer();
	m_pParticleRC->ClearContainer();
}
