//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------
#include <ctime> // for seeding rand
//#include <vld.h>

#include "./Application.h"
#include "./MainWindow.h"
#include "../PrecisionTimer.h"
#include "../Log.h"
#include "../EventModule/EventManager.h"
#include "../RendererModule/Renderer.h"
#include "../InputModule/InputManager.h"
#include "../LevelModule/ObjectRegistry.h"
#include "../EventModule/MessageSystem.h"
#include "../InputModule/InputController.h"
#include "../Log.h"
#include "../GameModule/Game.h"

// MEMORY LEAK THINGS
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//--------------------------------------------------------------------------------
CApplication* CApplication::s_pInstance = nullptr;
//--------------------------------------------------------------------------------
void ShutdownWinMain(CApplication *_app);
//--------------------------------------------------------------------------------
int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,// -debug
	_In_ int nShowCmd) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nShowCmd);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1);

	LogSetUp(L"MurphyGaming Tutorial Series WWise");

	Log("***************************************************************************");
	Log("Normally it is best to ONLY log out success of any operation while debugging");
	Log("This example, left in some success logs to show the order of things");
	Log("A good rule is always log errors");
	Log("log success while getting something up and running, but remove success logs before checking into the main build");
	Log("***************************************************************************\n");

	//Seed rand
	srand((unsigned)time(0) );
	CApplication* pApp = nullptr;
	
	// Initialize Event System
	Events::CEventManager::CreateInstance();
	Events::MessageSystem::CreateInstance();

	pApp = CreateApplication();

	if (!pApp->Init() ) {
		ShutdownWinMain(pApp);
		return 1;
	}
	pApp->SceneInit();
	pApp->Run();

	pApp->SceneEnd();

	ShutdownWinMain(pApp);

	return 0;
}
//--------------------------------------------------------------------------------
void ShutdownWinMain(CApplication *_app) {
	SAFE_SHUT(_app);

	Events::EventManager()->Shutdown();
	Events::CEventManager::DestroyInstance();
	Events::MessageSystem::DestroyInstance();

	LogShutDown();
	//_CrtDumpMemoryLeaks();
}
//--------------------------------------------------------------------------------
CApplication::CApplication() :
	m_szWindowTitle(L"Default window name"),
	m_nWindowWidth(1920),
	m_nWindowHeight(1080),
	m_bIsActive(true),
	m_Windowed(1) {
	if( s_pInstance ) {
		Log("Will Not open Application already running");
		return;//throw CError("Application object already created!\n");
	}
	s_pInstance = this;
}
//--------------------------------------------------------------------------------
CApplication::~CApplication() {
	//CInputManager::DestroyInstance();
	delete m_window;
	m_window = nullptr;
	//SAFE_SHUT(m_dxRender);
	//SAFE_SHUT(m_pRenderer);
	delete m_Objects;
	m_Objects = nullptr;
	SAFE_SHUT(m_controller);
	SAFE_SHUT(m_input);
	//delete m_pGame;
}
//--------------------------------------------------------------------------------
bool CApplication::Init() {
	m_Objects = new ObjectRegistry;
	
	InternalSetDefaultModules();

	m_controller = m_Objects->CreateModule("InputController");
	m_input = dynamic_cast<IWindProc *>(m_Objects->CreateModule("Input") );

	if (!m_input) {
		Log("Input is not based off class IWindProc exiting");
		return false;
	}
	m_input->Init();
	m_controller->Init();


	m_window = new CMainWindow(m_nWindowWidth, m_nWindowHeight, m_szWindowTitle);
	m_window->Initialize(this);
	
	return true;
}
//--------------------------------------------------------------------------------
HWND CApplication::GetHandle()
{
	return m_window->GetHWnd();
}
void CApplication::Run() {
	CPrecisionTimer::Init();
	while( true ) {
		// Message pump
		while (m_window->HasMessages()) {
			if (false == m_window->MessagePump())
				return;
		}

		CPrecisionTimer::GlobalUpdate();
		Update(CPrecisionTimer::GetDeltaTime() );
	}
}
//--------------------------------------------------------------------------------
void CApplication::Update(float _delta) {
	if (m_bIsActive) {
		static float totalTime = 0;
		totalTime += _delta;
		DoFrame(_delta, totalTime);

	}
	else {
		static float totalTime = 0;
		totalTime += _delta;
		DoIdleFrame(_delta, totalTime);
	}
}
void CApplication::DoFrame( float timeDelta, float timeTotal ) {
	m_Objects->Update(timeDelta);
	m_input->Update(timeDelta);
}
//--------------------------------------------------------------------------------
void CApplication::DoIdleFrame( float timeDelta, float timeTotal ) {}
//--------------------------------------------------------------------------------
LRESULT CApplication::WindowProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) {
	LRESULT notHandled = m_input->WindowProc(_hWnd, _msg, _wParam, _lParam);
	///////////////////////////////////////////////////////////////////////////////
	////Debug hint for wndproc info:
	////If you need to find a message filter the messages you do not care about
	//switch (_msg) {
	//case WM_NCHITTEST:
	//case WM_MOUSEMOVE:
	//case WM_GETICON:
	//case WM_PAINT:
	//case WM_SETCURSOR:
	//	break;
	//default:
	//	LogWndProc(_msg);
	//}
	///////////////////////////////////////////////////////////////////////////////

	//The only input we need to see is alt + enter and alt + tab
	if (notHandled == 0 && WM_SYSKEYDOWN != _msg)
		return 0;

	//If we have no input see what our message is
	switch (_msg) {
		case WM_NCACTIVATE: {
			if (0 == _wParam && 0 != m_window->GetHWnd()) {
				//Log(STRINGIZE(WM_NCACTIVATE), " Windowed ", m_dxRender->GetWindowed());
				if (((CGame*)this)->GetRenderer()->GetWindowed() == false) 
				{
					((CGame*)this)->GetRenderer()->SetWindowed(m_window->GetWindowWidth(), m_window->GetWindowHeight(), true);
				}
			}
		} break;
		case WM_SETFOCUS: {
			//Log(STRINGIZE(WM_SETFOCUS), " Windowed ", m_dxRender->GetWindowed());
			if (((CGame*)this)->GetRenderer()->GetWindowed() == false)
			{
				((CGame*)this)->GetRenderer()->SetWindowed(m_window->GetWindowWidth(), m_window->GetWindowHeight(), ((CGame*)this)->GetRenderer()->GetWindowed());
			}
		} break;
		case WM_SYSKEYDOWN:{
			switch (_wParam) {
			case VK_RETURN: {
				if ((_lParam & (1 << 29)) != 0) {
					//Log("Alt Enter width ", m_window->GetWindowWidth(), " height ", m_window->GetWindowHeight());
					((CGame*)this)->GetRenderer()->SetWindowed(m_window->GetWindowWidth(), m_window->GetWindowHeight(), !((CGame*)this)->GetRenderer()->GetWindowed());
					//Log("____________________________________________________________________");
				}
			}break;
			};
		} break;
		case WM_SIZE: {
			//Log(STRINGIZE(WM_SIZE));
			((CGame*)this)->GetRenderer()->ResizeBuffers(m_window->GetWindowWidth(), m_window->GetWindowHeight());
		} break;
		//to remove the beep from alt tabbing from fullscreen to windowed
		case WM_SYSCOMMAND: {
			switch (_wParam) {
				case SC_KEYMENU:{
					return 1;
				}
			};
		} break;
		// This message is sent when a window has been destroyed.
		case WM_DESTROY: {
			PostQuitMessage(0);
			return(0);
		} break;
	};

	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}
//--------------------------------------------------------------------------------
void CApplication::SceneInit() {}
//--------------------------------------------------------------------------------
void CApplication::SceneEnd() {
	m_Objects->ShutDown();
	m_controller->ShutDown();
}
//--------------------------------------------------------------------------------
IBaseObject *CApplication::CreateObject(const char *_key) {
	return m_Objects->CreateObject(_key);
}
//--------------------------------------------------------------------------------
RenderNode *CApplication::CreateRenderNode(const char *_key) {
	RenderNode *Node = m_Objects->CreateNode(_key);
	return Node;
}
//--------------------------------------------------------------------------------
void CApplication::InternalSetDefaultModules() {
	//Make sure all defaults are set first
	m_Objects->RegisterModule<InputController>("InputController");
	m_Objects->RegisterModule<CInputManager>("Input");
	SetDefaultModules();
}
//--------------------------------------------------------------------------------
void  CApplication::SetDefaultModules() {}
//--------------------------------------------------------------------------------
float  CApplication::GetWindowWidth() {
	return static_cast<float>(m_window->GetWindowWidth() );
}
//--------------------------------------------------------------------------------
float  CApplication::GetWindowHeight() {
	return static_cast<float>(m_window->GetWindowHeight() );
}
//--------------------------------------------------------------------------------
bool CApplication::ShutDown() {
	SAFE_SHUT(m_controller);
	SAFE_SHUT(m_input);
	return true;
}
