#pragma once
#include "Component.h"

class blockSpawnerScript :
	public Component
{
	GameObject* curruntBlock;
	GameObject* nextBlock;

public:
	blockSpawnerScript(GameObject* gameObject);
	~blockSpawnerScript();

	void start();

	void update();
};

