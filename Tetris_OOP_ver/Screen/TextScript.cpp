#include "TextScript.h"
#include "GameObject.h"
#include "Transform.h"
#include "ScoreScript.h"
#include "BlockScript.h"

TextScript::TextScript(GameObject* gameObject)
	:Component(gameObject), screen(Screen::getInstance()),
	score(nullptr), block(nullptr)
{

}


TextScript::~TextScript()
{
}

void TextScript::start()
{
	score = static_cast<ScoreScript *>(GameObject::Find("score")->getComponent<ScoreScript>());
}

void TextScript::update()
{

	block = static_cast<BlockScript *>(GameObject::Find("block")->getComponent<BlockScript>());
	Vector2 parentPos = gameObject->getParent()->getTransform()->getPosition();

	// 게임화면 Panel
	if (gameObject->getParent() == GameObject::FindWithTag("nextPanel"))
	{
		//기본 UI
		string nextBlockT = "next";
		string ScoreT = "Score : " + to_string(score->getScore());
		string PlayTimeT = "PlayTime : " + to_string(score->getPlayTime()) + " s";
		string MaxComboT = "MaxCombo : " + to_string(score->getMaxCombo());
		string funtion1T = "Left, Right : Move Block";
		string funtion2T = "Up   : Rotate Block";
		string funtion3T = "Down : Block Once Down";
		string funtion4T = "Space : Block straight Down";
		string funtion5T = "R : GuideLine On / Off";

		screen.draw(nextBlockT.c_str(), 1, nextBlockT.size(), Vector2{1,0} + parentPos);
		screen.draw(ScoreT.c_str(), ScoreT.size(), 1, Vector2{ 1,6 } + parentPos);
		screen.draw(PlayTimeT.c_str(), PlayTimeT.size(), 1, Vector2{ 1,7 } + parentPos);
		screen.draw(MaxComboT.c_str(), MaxComboT.size(), 1, Vector2{ 1,8 } +parentPos);
		screen.draw(funtion1T.c_str(), funtion1T.size(), 1, Vector2{ 1,10 } + parentPos);
		screen.draw(funtion2T.c_str(), funtion2T.size(), 1, Vector2{ 1,11 } + parentPos);
		screen.draw(funtion3T.c_str(), funtion3T.size(), 1, Vector2{ 1,12 } + parentPos);
		screen.draw(funtion4T.c_str(), funtion4T.size(), 1, Vector2{ 1,13 } + parentPos);
		screen.draw(funtion5T.c_str(), funtion5T.size(), 1, Vector2{ 1,19 } +parentPos);

		// 스킬 1, 2, 3

		string skill1T = "Skill1";
		string skill2T = "Skill2";
		string skill3T = "Skill3";
		string skill1CoolT = to_string(block->getSkillCoolTime(1));
		string skill2CoolT = to_string(block->getSkillCoolTime(2));
		string skill3CoolT = to_string(block->getSkillCoolTime(3));

		if (block->getSkillCoolTime(1) == 0) // 스킬 사용이 가능한 경우 출력내용
			skill1CoolT = "OK!";
		else //	쿨타임 시의 출력 내용
			skill1CoolT = to_string(block->getSkillCoolTime(1));

		if (block->getSkillCoolTime(2) == 0)
			skill2CoolT = "OK!";
		else
			skill2CoolT = to_string(block->getSkillCoolTime(2));

		if (block->getSkillCoolTime(3) == 0)
			skill3CoolT = "OK!";
		else
			skill3CoolT = to_string(block->getSkillCoolTime(3));

		string skill1info_T = "Q, Skill1 : ";
		string skill2info_T = "W, Skill2 : ";
		string skill3info_T = "E, Skill3 : ";

		if (score->getScore() < 1000) // 스킬 사용 조건을 충족하지 못했을때의 출력 내용
		{
			skill1info_T += "Score ( " + to_string(score->getScore()) + " / 1000 )";
			skill1CoolT = "XX";
		}
		else // 스킬 사용 조건을 충족했을때의 내용
			skill1info_T += "3s StopBlock";

		if (score->getMaxCombo() < 4)
		{
			skill2info_T += "MaxCombo ( " + to_string(score->getMaxCombo()) + " / 4 )";
			skill2CoolT = "XX";
		}
		else
			skill2info_T += "Break ThreeBlockLine";

		if (score->getPlayTime() < 120)
		{
			skill3info_T += "PlayTime ( " + to_string(score->getPlayTime()) + " / 120 )";
			skill3CoolT = "XX";
		}
		else
			skill3info_T += "Map Clear";

		screen.draw(skill1T.c_str(), skill1T.size(), 1, Vector2{ 9,1 } +parentPos);
		screen.draw(skill2T.c_str(), skill2T.size(), 1, Vector2{ 16,1 } +parentPos);
		screen.draw(skill3T.c_str(), skill3T.size(), 1, Vector2{ 23,1 } +parentPos);
		screen.draw(skill1CoolT.c_str(), skill1CoolT.size(), 1, Vector2{ 11,3 } +parentPos);
		screen.draw(skill2CoolT.c_str(), skill2CoolT.size(), 1, Vector2{ 18,3 } +parentPos);
		screen.draw(skill3CoolT.c_str(), skill3CoolT.size(), 1, Vector2{ 25,3 } +parentPos);
		screen.draw(skill1info_T.c_str(), skill1info_T.size(), 1, Vector2{ 1, 15 } +parentPos);
		screen.draw(skill2info_T.c_str(), skill2info_T.size(), 1, Vector2{ 1, 16 } +parentPos);
		screen.draw(skill3info_T.c_str(), skill3info_T.size(), 1, Vector2{ 1, 17 } +parentPos);
	}

	// 끝났을 때 GameOver Panel
	if (gameObject->getParent() == GameObject::FindWithTag("gameOverPanel"))
	{
		string ScoreT = "Total Score : " + to_string(score->getScore());
		string PlayTimeT = "Total PlayTime : " + to_string(score->getPlayTime()) + " s";
		string MaxComboT = "MaxCombo : " + to_string(score->getMaxCombo());
		string MessageT = "Thank you for play!!";
		string exitT = "Exit : ESC";

		screen.draw(ScoreT.c_str(), ScoreT.size(), 1, Vector2{ 1,9 } +parentPos);
		screen.draw(PlayTimeT.c_str(), PlayTimeT.size(), 1, Vector2{ 1,10 } +parentPos);
		screen.draw(MaxComboT.c_str(), MaxComboT.size(), 1, Vector2{ 1,11 } +parentPos);
		screen.draw(MessageT.c_str(), MessageT.size(), 1, Vector2{ 6,16 } +parentPos);
		screen.draw(exitT.c_str(), exitT.size(), 1, Vector2{ 11,18 } +parentPos);
	}

}
