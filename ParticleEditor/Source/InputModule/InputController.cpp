#include "./InputController.h"
#include "../EventModule/MessageSystem.h"
#include "../Log.h"
#include <Windows.h>

//--------------------------------------------------------------------------------
bool InputController::Init() {
	using namespace Events;
	MessageSystem *ms = MessageSystem::GetInstance();
	if (!ms) {
		Log("cannot create input controller before the Message system has been created");
		return false;
	}
	//Down
	AddKeyEvent(m_keyDown, "DirForward", KeyInfo('W', MapType::DOWN) );
	AddKeyEvent(m_keyDown, "DirBack", KeyInfo('S', MapType::DOWN) );
	AddKeyEvent(m_keyDown, "DirLeft", KeyInfo('A', MapType::DOWN) );
	AddKeyEvent(m_keyDown, "DirRight", KeyInfo('D', MapType::DOWN) );
	AddKeyEvent(m_keyDown, "MouseLButton", KeyInfo(VK_LBUTTON, MapType::DOWN));
	AddKeyEvent(m_keyDown, "MouseRButton", KeyInfo(VK_RBUTTON, MapType::DOWN));
	AddKeyEvent(m_keyDown, "MouseMButton", KeyInfo(VK_MBUTTON, MapType::DOWN));
	AddKeyEvent(m_keyDown, "ThrustButton", KeyInfo('2', MapType::DOWN));
	AddKeyEvent(m_keyDown, "KickButton", KeyInfo('F', MapType::DOWN));

	AddKeyEvent(m_keyDown, "PrintButton", KeyInfo('Z', MapType::DOWN));
	AddKeyEvent(m_keyDown, "DirUp", KeyInfo('E', MapType::DOWN));
	AddKeyEvent(m_keyDown, "DirDown", KeyInfo('Q', MapType::DOWN));
	AddKeyEvent(m_keyDown, "ShiftDown", KeyInfo(VK_SHIFT, MapType::DOWN));
	AddKeyEvent(m_keyUp, "TheToggle", KeyInfo('T', MapType::UP));
	AddKeyEvent(m_keyUp, "ShiftUp", KeyInfo(VK_SHIFT, MapType::UP));
	AddKeyEvent(m_keyUp, "ToMainMenu", KeyInfo(VK_F9, MapType::UP));
	AddKeyEvent(m_keyUp, "LockOn", KeyInfo(VK_SPACE, MapType::UP));
	
	ms->RegisterMessage<InputController, const EVENTID&, const EVENTID>("GetKeyDown", this, &InputController::GetKeyDownEvent);
	ms->RegisterMessage<InputController, const EVENTID&, const EVENTID>("GetKeyPress", this, &InputController::GetKeyPressEvent);
	ms->RegisterMessage<InputController, const EVENTID&, const EVENTID>("GetKeyUp", this, &InputController::GetKeyUpEvent);
	ms->RegisterMessage<InputController, bool, EVENTID>("RemoveKey", this, &InputController::UnRegisterEvent);
	ms->RegisterMessage<InputController, bool, const EVENTID, const unsigned char>("SwitchKeyDown", this, &InputController::SwithKeyDownEvent);

	return true;
}
//--------------------------------------------------------------------------------
bool InputController::ShutDown() {
	m_keyDown.clear();
	m_keyPress.clear();
	m_keyUp.clear();
	m_codes.clear();

	return true;
}
//--------------------------------------------------------------------------------
const Events::EVENTID & InputController::GetKeyDownEvent(const Events::EVENTID _key) {
	ConstMapItr itr = m_keyDown.find(_key);
	if (itr == m_keyDown.end()) {
		Log("No event at ", _key);
		static Events::EVENTID junk;
		return junk;
	}
	return itr->second;
}
//--------------------------------------------------------------------------------
const Events::EVENTID & InputController::GetKeyPressEvent(const Events::EVENTID _key) {
	ConstMapItr itr = m_keyPress.find(_key);
	if (itr == m_keyPress.end()) {
		Log("No event at ", _key);
		static Events::EVENTID junk;
		return junk;
	}
	return itr->second;
}
//--------------------------------------------------------------------------------
const Events::EVENTID & InputController::GetKeyUpEvent(const Events::EVENTID _key) {
	ConstMapItr itr = m_keyUp.find(_key);
	if (itr == m_keyUp.end() ) {
		Log("No event at ", _key);
		static Events::EVENTID junk;
		return junk;
	}
	return itr->second;
}
//--------------------------------------------------------------------------------
void InputController::AddKeyEvent(Map &_store, const Events::EVENTID _key, KeyInfo _info) {
	MapItr itr = _store.find(_key);
	if (itr == _store.end() ) {
		using namespace Events;
		MessageSystem *ms = MessageSystem::GetInstance();
		if (!ms) {
			Log("No Message system cannot AddKeyEvent");
			return;
		}
		Data data;
		data.first = _key;
		std::string msgKey;
		switch (_info.type) {
		case MapType::DOWN:
			msgKey = "BindKeyDownEvent";
			break;
		case MapType::PRESS:
			msgKey = "BindKeyPressEvent";
			break;
		case MapType::UP:
			msgKey = "BindKeyReleaseEvent";
			break;
		default:
			Log("Bad Type sent into ", __FUNCTION__);
		};
		data.second = ms->Call<EVENTID, const unsigned char, EVENTID>(msgKey, _info.vkCode, _key);
		
		Code code(data.first, _info);

		m_codes.insert(code);
		_store.insert(data);
		return;
	}

	Log("Already added a keydown event with this key ", _key);
}
//--------------------------------------------------------------------------------
bool InputController::UnRegisterEvent(const Events::EVENTID _key) {
	CodeItr itr = m_codes.find(_key);
	if (itr != m_codes.end()) {
		Events::MessageSystem *ms = Events::MessageSystem::GetInstance();
		if (!ms) {
			Log("No Message system cannot bind key");
			return false;
		}
		ms->Call<bool, const unsigned char>("DeActivateKey", itr->second.vkCode);
		
		switch (itr->second.type) {
		case MapType::DOWN:
			m_keyDown.erase(_key);
			break;
		case MapType::PRESS:
			m_keyPress.erase(_key);
			break;
		case MapType::UP:
			m_keyUp.erase(_key);
			break;
		default:
			Log("Bad Type sent into ", __FUNCTION__);
		};
		m_codes.erase(itr);
		
		return true;
	}

	Log("Message: ", _key, "Could not be found to earase");
	return false;
}
//--------------------------------------------------------------------------------
bool InputController::SwithKeyDownEvent(const Events::EVENTID _key, const unsigned char _vk) {
	CodeItr itr = m_codes.find(_key);
	if (itr != m_codes.end()) {
		using namespace Events;
		MessageSystem *ms = MessageSystem::GetInstance();
		if (!ms) {
			Log("No Message system cannot bind key");
			return false;
		}
		ms->Call<bool, const unsigned char>("DeActivateKey", itr->second.vkCode);
		itr->second.vkCode = _vk;
		std::string msgKey;
		switch (itr->second.type) {
			case MapType::DOWN:
				msgKey = "BindKeyDownEvent";
				break;
			case MapType::PRESS:
				msgKey = "BindKeyPressEvent";
				break;
			case MapType::UP:
				msgKey = "BindKeyReleaseEvent";
				break;
			default:
				Log("Bad Type sent into ", __FUNCTION__);
		};
		ms->Call<EVENTID, const unsigned char, EVENTID>(msgKey, _vk, _key);
		return true;
	}
	Log("Could not find the event Key ", _key, " in ", __FUNCTION__);
	return false;
}
//--------------------------------------------------------------------------------
InputController::KeyInfo::KeyInfo(const unsigned char _vk, MapType _type) : vkCode(_vk), type(_type) {}
//--------------------------------------------------------------------------------
