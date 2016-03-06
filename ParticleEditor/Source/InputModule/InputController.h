#pragma once
//--------------------------------------------------------------------------------
#include "../EventModule/Event.h"
#include "../LevelModule/IBaseModule.h"
#include <map>
#include <vector>
#include <stdint.h>
//--------------------------------------------------------------------------------
class InputController :public IBaseModule {
public:
	~InputController() {}
	bool Init() override;
	bool ShutDown() override;
	void Update(float) {}
	//Messages accepted
	//"GetKeyDown" -  return const EVENTID&, param - const EVENTID 
	//"GetKeyPress" - return const EVENTID&, param - const EVENTID 
	//"GetKeyUp" - return const EVENTID&, param - const EVENTID 
	//"RemoveKey" - return bool, params - const EVENTID 
	//"GetMousePos" - params - float & w, float & h
	//"GetMousePrevPos" - params - float & w, float & h 
protected:
	//Message Handlers
	const Events::EVENTID & GetKeyDownEvent(const Events::EVENTID _event);
	const Events::EVENTID & GetKeyPressEvent(const Events::EVENTID _event);
	const Events::EVENTID & GetKeyUpEvent(const Events::EVENTID _event);

	bool UnRegisterEvent(const Events::EVENTID _event);
	bool SwithKeyDownEvent(const Events::EVENTID _event, const unsigned char _vk);

	enum class MapType : int8_t { UP, DOWN, PRESS, MAX_MAPS };

	struct KeyInfo {
		KeyInfo(const unsigned char _vk, MapType _type);
		unsigned char vkCode;
		MapType type;
	};

	typedef std::map<Events::EVENTID, Events::EVENTID>					Map;
	typedef std::map<Events::EVENTID, Events::EVENTID>::iterator		MapItr;
	typedef std::map<Events::EVENTID, Events::EVENTID>::const_iterator	ConstMapItr;
	typedef std::map<Events::EVENTID, KeyInfo>::iterator				CodeItr;
	typedef std::pair<Events::EVENTID, Events::EVENTID>					Data;
	typedef std::pair<Events::EVENTID, KeyInfo>							Code;

	void AddKeyEvent(Map &_store, const Events::EVENTID _key, KeyInfo _info);

	std::map<Events::EVENTID, Events::EVENTID> m_keyDown;
	std::map<Events::EVENTID, Events::EVENTID> m_keyPress;
	std::map<Events::EVENTID, Events::EVENTID> m_keyUp;
	std::map<Events::EVENTID, KeyInfo> m_codes;
};
//--------------------------------------------------------------------------------