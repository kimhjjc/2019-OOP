#pragma once
#include "Component.h"
#include "blockSpawnerScript.h"
#include "BlockScript.h"
#include "PanelDrawScript.h"
#include "TextScript.h"
#include "ScoreScript.h"

class GameObject;

class MyScript : public Component
{
public:
	MyScript(GameObject* gameObject);

protected:	

	void start() {}

	void update() {}
};

