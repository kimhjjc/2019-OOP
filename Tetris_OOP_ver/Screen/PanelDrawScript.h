#pragma once
#include "Component.h"
#include "Utils.h"

class PanelDrawScript :
	public Component
{

	Screen& screen;

public:
	PanelDrawScript(GameObject* gameObject);
	~PanelDrawScript();


	void start();

	void update();
};

