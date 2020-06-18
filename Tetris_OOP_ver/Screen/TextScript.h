#pragma once
#include "Component.h"
#include "Utils.h"

class ScoreScript;
class BlockScript;

class TextScript :
	public Component
{
	ScoreScript* score;
	BlockScript* block;
	Screen& screen;

public:
	TextScript(GameObject* gameObject);
	~TextScript();


	void start();

	void update();
};

