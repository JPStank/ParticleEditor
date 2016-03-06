//--------------------------------------------------------------------------------
// CApplication
// Written By: Justin Murphy
//--------------------------------------------------------------------------------
#ifndef Application_h
#define Application_h
//--------------------------------------------------------------------------------
#include "./IWindProc.h"
#include "../LevelModule/ObjectRegistry.h"
#include "../EventModule/Event.h"
//--------------------------------------------------------------------------------
class CMainWindow;
class Renderer;
class CRenderer;
class CCrispyGame;
struct __declspec(novtable) IBaseModule;
//--------------------------------------------------------------------------------
class CApplication :public IWindProc {
public:
	CApplication();
	virtual ~CApplication();

	// Init the application.  All user-level init should go here
	bool Init() override; 
	bool ShutDown() override;
	void Update(float _delta) override;

	// Run the application.  This shouldn't need to change
	virtual void Run();

	// Handles all code that should be run each frame
	// timeDelta == amount of time (in seconds) since last call
	// timeTotal == amount of time (in seconds) since start
	virtual void DoFrame( float timeDelta, float timeTotal );
	// update but when window doesn't have focus
	virtual void DoIdleFrame( float timeDelta, float timeTotal );

	// Scene Init is called by Init.  This way a user-level class 
	// doesn't need to worry about setting up D3D or handling any 
	// errors during setup.
	virtual void SceneInit();
	virtual void SceneEnd();

	virtual void Pause();
	virtual void UnPause();
	
	static CApplication* GetApplication();

	IBaseObject *CreateObject(const char *_key);
	RenderNode *CreateRenderNode(const char *_key);

	int GetWindWidthDirty() { return m_nWindowWidth; }
	int GetWindHeightDirty() { return m_nWindowHeight; }

protected:
	const wchar_t*	m_szWindowTitle;
	int			m_nWindowWidth;
	int			m_nWindowHeight;
	int			m_bIsActive : 1;
	int			m_bIsDone : 1;
	int			m_Windowed : 1;

	static CApplication*	s_pInstance;

	LRESULT WindowProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) override;

	template<typename T>
	unsigned int RegisterObject(const char* _key) {
		return m_Objects->RegisterType<T>(_key);
	}
	template<typename T>
	unsigned int RegisterRenderObject(const char* _key) {
		return m_Objects->RegisterRenderType<T>(_key);
	}
	template<typename T>
	unsigned int RegisterModule(const char* _key) {
		return m_Objects->RegisterModule<T>(_key);
	}

	//Modules defined to override
	// call with m_Objects->RegisterModule<TYPE>("Key");
	//"InputController" - IBaseModule
	//"Renderer" - Renderer
	//"Input" - IWindProc
	virtual void SetDefaultModules();
	
	float GetWindowWidth();
	float GetWindowHeight();

	ObjectRegistry *m_Objects;
public:
	HWND GetHandle();
private:
	CMainWindow *m_window;
	//Renderer *m_dxRender;
	//CRenderer* m_pRenderer;
	IBaseModule *m_controller;
	IWindProc *m_input;
	//CCrispyGame* m_pGame;

	void InternalSetDefaultModules();
};
//--------------------------------------------------------------------------------
inline void CApplication::Pause() { m_bIsActive = false; }
//--------------------------------------------------------------------------------
inline void CApplication::UnPause() { m_bIsActive = true; }
//--------------------------------------------------------------------------------
inline CApplication* CApplication::GetApplication() { return s_pInstance; }
//--------------------------------------------------------------------------------
// This must be defined by any client that uses this framework
CApplication*	CreateApplication(); // returns a pointer to a valid application object
//--------------------------------------------------------------------------------

#endif // Application_h
//--------------------------------------------------------------------------------