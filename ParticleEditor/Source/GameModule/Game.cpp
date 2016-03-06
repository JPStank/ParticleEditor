//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------
#include "./Game.h"
#include "../LevelModule/Camera.h"
#include "../EventModule/EventManager.h"
#include "../EventModule/MessageSystem.h"
#include "../Log.h"
#include "./GameController.h"

#include "../RendererModule/Renderer.h"
#include "../AssetModule/AssetManager.h"
#include "../EmitterModule/EmitterManager.h"
#include "../LevelModule/CLevel.h"
#include "../States/IState.h"
#include "../States/LevelState.h"


//--------------------------------------------------------------------------------
bool CGame::m_bShowCursor = true;
bool CGame::m_bKeepCursorPos = true;

//--------------------------------------------------------------------------------
CGame::CGame() :
CApplication(),
volume(50.f),
AudioScaling(1.f) {
	m_szWindowTitle = L"Stank's Radical Particle Editor 2k16 Edition";
	m_nWindowWidth = 1280;
	m_nWindowHeight = 720;
	m_pAssetManager = nullptr;
	m_pEmitterManager = nullptr;
	m_pRenderer = nullptr;

	m_pCurrState = m_pNextState = nullptr;
}
//--------------------------------------------------------------------------------
void CGame::SceneInit()
{
	// init systems pls
	m_pAssetManager = new CAssetManager();
	if (m_pAssetManager->Initialize() == false)
	{
		m_pAssetManager->ShutDown();
		delete m_pAssetManager;
		m_pAssetManager = nullptr;
		Log("ERROR IN " __FUNCTION__);
		return; // maybe log stuff too
	}

	m_pEmitterManager = new CEmitterManager();
	m_pRenderer = new CRenderer();
	if (m_pRenderer->Initialize(m_nWindowWidth, m_nWindowHeight, GetHandle()) == false)
	{
		m_pRenderer->ShutDown();
		delete m_pRenderer;
		m_pRenderer = nullptr;
		Log("ERROR IN " __FUNCTION__);
		return;
	}
	m_pAssetManager->SetDevice(m_pRenderer->GetDevice());
	m_pAssetManager->SetWindowSize((unsigned int)m_nWindowWidth, (unsigned int)m_nWindowHeight);



	using namespace Events;

	//Register scene objects
	RegisterObject<Camera>("Camera");
	RegisterRenderObject<PerFrame>("view");

	MessageSystem *ms = MessageSystem::GetInstance();
	if (!ms) {
		Log("Need a message system in", __FUNCTION__);
		return;
	}

	ms->RegisterMessage<CGame, void, IState*>("SetNextState", this, &CGame::SetNextState);

	//Make the view
	Camera *camera = dynamic_cast<Camera *>(CreateObject("Camera"));
	camera->LookAt(DirectX::XMFLOAT3(0.0f, 1.0f, -5.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	camera->SetLens(DirectX::XM_PIDIV4, GetWindowWidth() / GetWindowHeight(), 0.01f, 1000.0f);
	camera->SetRenderNode(CreateRenderNode("view"));
	//register with the sound system
	// also register with renderer
	m_pRenderer->AddCamera(camera);
	m_pCam = camera;

	m_pCurrState = new CLevelState("ParticleLevel");
	m_pCurrState->Enter(this, camera);
}
//--------------------------------------------------------------------------------
void CGame::Update(float timeDelta, float totalTime)
{
	using namespace DirectX;
	if (m_pCurrState)
	{
		m_pCurrState->Update(timeDelta);
	}

	if (GetAsyncKeyState(VK_ESCAPE) & 0x1)
	{
		StopGame();
	}

	if (m_pNextState)
	{
		ChangeState(m_pNextState);
	}
	m_pRenderer->Update(timeDelta);
}
//--------------------------------------------------------------------------------
void CGame::SetDefaultModules() {
	m_Objects->RegisterModule<GameController>("InputController");
}
//--------------------------------------------------------------------------------
void CGame::StopGame(void) {
	PostQuitMessage(0);
}

void CGame::ChangeState(IState* _state)
{
	if (!_state)
	{
		StopGame();
	}
	else
	{
		m_pCurrState->Exit();
		delete m_pCurrState;
		m_pAssetManager->ShutDown();
		m_pAssetManager->SetDevice(m_pRenderer->GetDevice());
		m_pAssetManager->SetWindowSize((unsigned int)m_nWindowWidth, (unsigned int)m_nWindowHeight);
		m_pCurrState = _state;
		m_pNextState = nullptr;
		m_pCurrState->Enter(this, m_pCam);
	}
}

void CGame::SetNextState(IState* _state)
{
	if (!_state)
	{
		StopGame();
	}
	else
	{
		m_pNextState = _state;
	}
}

//--------------------------------------------------------------------------------
void CGame::SceneEnd() {
	using namespace Events;

	MessageSystem *ms = MessageSystem::GetInstance();
	if (ms) {
		ms->UnregisterRegisterMessage("SetNextState");
	}


	m_pCurrState->Exit();
	delete m_pCurrState;
	m_pCurrState = nullptr;

	// THIS MUST HAPPEN AFTER STATE SHUTS DOWN
	super::SceneEnd(); // this function deletes the camera, like a fucking asshole

	m_pEmitterManager->ShutDown();
	delete m_pEmitterManager;
	m_pEmitterManager = nullptr;

	m_pAssetManager->ShutDown();
	delete m_pAssetManager;
	m_pAssetManager = nullptr;


	m_pRenderer->ShutDown();
	delete m_pRenderer;
	m_pRenderer = nullptr;
}
//--------------------------------------------------------------------------------
void CGame::Pause() {
	m_bIsActive = false;
}
//--------------------------------------------------------------------------------
void CGame::UnPause() {
	m_bIsActive = true;
}
//--------------------------------------------------------------------------------
void CGame::DoFrame(float fDeltaTime, float totalTime) {
	Events::EventManager()->ProcessEvents();

	Update(fDeltaTime, totalTime);
	super::DoFrame(fDeltaTime, totalTime);
}
//--------------------------------------------------------------------------------
void CGame::DoIdleFrame(float timeDelta, float totalTime) {
	// for debugging continue to run and update the game
#ifdef _DEBUG
	Update(timeDelta, totalTime);
#endif
}
//--------------------------------------------------------------------------------
CGame::~CGame() {}
//--------------------------------------------------------------------------------
CApplication* CreateApplication() {
	return new CGame;
}