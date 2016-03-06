#include "./GameController.h"
#include "../EventModule/MessageSystem.h"
#include "../Log.h"
#include <Windows.h>

bool GameController::Init() {

	using namespace Events;
	MessageSystem *ms = MessageSystem::GetInstance();
	if (!ms) {
		Log("cannot Init GameController before the Message system has been created");
		return false;
	}
	AddKeyEvent(m_keyPress, "QUIT", KeyInfo(VK_ESCAPE, MapType::PRESS) );

	//Get the default Keys set up
	InputController::Init();

	return true;
}