#pragma once
#include <iostream>
#include <string>
#include "Utils.h"

using namespace std;

class Screen;
class Input;

class GameEngine {
	Screen& screen;

	static GameEngine* instance;

	GameEngine();

public:
	static GameEngine& getInstance();

	void mainLoop();
};
