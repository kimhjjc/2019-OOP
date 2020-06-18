#include "ScoreScript.h"
#include "GameObject.h"
#include <time.h>

ScoreScript::ScoreScript(GameObject* gameObject)
	:Component(gameObject)
{

}

ScoreScript::~ScoreScript()
{
}

void ScoreScript::start()
{
	score = 0;	// 게임 스코어
	startTime = clock();
	playTime = 0;	
	maxCombo = 0;
}

void ScoreScript::update()
{
	if (GameObject::FindWithTag("nextPanel"))
		playTime = (clock() - startTime) / 1000;
}