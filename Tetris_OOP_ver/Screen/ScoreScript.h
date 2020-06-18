#pragma once
#include "Component.h"

class ScoreScript :
	public Component
{
	int score;
	int maxCombo;

	int startTime;
	int playTime;

	ScoreScript();

public:
	ScoreScript(GameObject* gameObject);
	~ScoreScript();

	void start();

	void update();

	void setScore(int index) { score += index; }
	int getScore() { return score; }

	void setMaxCombo(int index) { maxCombo = index; }
	int getMaxCombo() { return maxCombo; }

	int getPlayTime() { return playTime; }

};

