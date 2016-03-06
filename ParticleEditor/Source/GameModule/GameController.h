#pragma once
//--------------------------------------------------------------------------------
#include "../InputModule/InputController.h"
//--------------------------------------------------------------------------------
class GameController : public InputController {
public:
	bool Init() override;
};