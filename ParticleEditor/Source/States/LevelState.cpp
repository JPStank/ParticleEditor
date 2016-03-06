/***********************************************
* Filename:           	LevelState.cpp
* Date:  	         	02/23/2016
* Mod. Date:    	   	02/23/2016
* Mod. Initials:		SH
* Author:	         	Samuel Hotchkiss
* Purpose:            	State that controls gameplay logic
************************************************/

#include "LevelState.h"
#include "../LevelModule/CLevel.h"
#include "../GameModule/Game.h"
#include "../EmitterModule/EmitterManager.h"
#include "../AssetModule/AssetManager.h"
#include "../RendererModule/Renderer.h"
#include "../EventModule/EventManager.h"
#include "../EventModule/MessageSystem.h"
#include "../LevelModule/Camera.h"

CLevelState::CLevelState()
{
	m_pLevel = nullptr;
}

CLevelState::CLevelState(std::string _sz)
{
	m_pLevel = nullptr;

	m_szLevelToLoad = _sz;
}

CLevelState::~CLevelState()
{

}

void CLevelState::Enter(CGame* _game, Camera* _cam)
{
	m_pRenderer = _game->GetRenderer();
	m_pEmitterManager = _game->GetEmitterManager();
	m_pAssMan = _game->GetAssetManager();

	using namespace Events;
	MessageSystem* ms = MessageSystem::GetInstance();

	m_pAssMan->Initialize();
	m_pEmitterManager->Initialize();

	m_pLevel = new CLevel();
	if (m_szLevelToLoad != "")
	{
		m_pLevel->Initialize(m_szLevelToLoad, _game);
	}
	else
	{
		m_pLevel->Initialize("testLevel", _game);
	}

	m_pLevel->AddCamera(_cam);
}

void CLevelState::Update(float _dt)
{
	m_pEmitterManager->Update(_dt);
	m_pLevel->Update(_dt);
}

void CLevelState::Exit()
{
	if (m_pLevel)
	{
		m_pLevel->ShutDown();
		delete m_pLevel;
		m_pLevel = nullptr;
	}

	if (m_pEmitterManager)
	{
		m_pEmitterManager->ShutDown();
	}

	if (m_pAssMan)
	{
		m_pAssMan->ShutDown();
	}

	if (m_pRenderer)
	{
		m_pRenderer->ClearContexts();
	}
}
