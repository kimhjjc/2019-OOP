#include "PanelDrawScript.h"
#include "GameObject.h"
#include "Transform.h"

PanelDrawScript::PanelDrawScript(GameObject* gameObject)
	:Component(gameObject), screen(Screen::getInstance())
{
	
}


PanelDrawScript::~PanelDrawScript()
{
}

void PanelDrawScript::start()
{
}

void PanelDrawScript::update()
{
	// 인게임의 Panel 정보를 받아서 그려줌
	if (gameObject == GameObject::FindWithTag("nextPanel"))
	{
		screen.drawRect(transform->getPosition() + Vector2{ -1,-1 }, 36 + 2, 21 + 2);
	}
	else if (gameObject == GameObject::FindWithTag("gameOverPanel"))
	{
		screen.drawRect(transform->getPosition() + Vector2{ -15,5 }, 60 + 2, 15 + 2);
	}

}
