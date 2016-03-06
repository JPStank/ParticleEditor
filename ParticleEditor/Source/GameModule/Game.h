//--------------------------------------------------------------------------------
// Game
// Written By: Justin Murphy
// 
// Overall game class, this class creates, holds and manipulates 
// all objects and any non framework subsystems
//--------------------------------------------------------------------------------
#ifndef Game_h
#define Game_h
//--------------------------------------------------------------------------------
#include "../WinMain/Application.h"
#include "../LevelModule/Entity3D.h"

class Entity3D;
class CRenderer;
class CLevel;
class CAssetManager;
class CEmitterManager;
class CObject;
class IState;
class Camera;

//--------------------------------------------------------------------------------
class CGame : public CApplication {
public:
	virtual void Pause();
	virtual void UnPause();

	CGame();
	~CGame();
private:
	const int& poop(const int) const;
	typedef CApplication super;

	// simple fps check to console - JS
	float			m_fElapsedTime;
	unsigned long	m_lFrames;
	unsigned long	m_lFPS;

	// modules go here

	CEmitterManager* m_pEmitterManager;

	CRenderer* m_pRenderer;

	IState* m_pCurrState;
	IState* m_pNextState;

	CAssetManager* m_pAssetManager;

	// audio stuff
	
	float				volume;
	//Adjust Audio Scale
	//NOTE: only do this to get the placeholder bank integrated with your engine,
	//once you are ready to recieve a real soundbank tell the Sound Engineer the scale you are using
	float				AudioScaling;

	static bool			m_bShowCursor;
	static bool			m_bKeepCursorPos;
	POINT				m_InitialCursorPos; // used to reset the cursor to the correct position after click is released

	virtual void DoFrame(float timeDelta, float totaltime);
	virtual void DoIdleFrame( float timeDelta, float totaltime );
	virtual void SceneInit();
	virtual void SceneEnd();
	void SetDefaultModules() override;
	void Update(float timeDelta, float totaltime);

	void StopGame(void);

	Camera* m_pCam;
public:

	CAssetManager* GetAssetManager() { return m_pAssetManager; }
	CRenderer* GetRenderer() { return m_pRenderer; }
	CEmitterManager* GetEmitterManager() { return m_pEmitterManager; }
	IState* GetCurrState() { return m_pCurrState; }
	void ChangeState(IState*);
	void SetNextState(IState*);

};
//--------------------------------------------------------------------------------
#endif // Game_h
//--------------------------------------------------------------------------------