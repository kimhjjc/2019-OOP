#pragma once
#include "Component.h"
#include "GridManager.h"
#include "ScoreScript.h"
#include <iostream>
#include <vector>
class BlockScript :
	public Component
{
	GridManager* gm;
	ScoreScript* score;

	struct BlockShape {
		string shape;
		int width;
		int height;
	};

	vector<BlockShape> candidates{
		{ "\xB2\xB2 \xB2 \xB2", 2, 3},	// ㄱ자
		{ "\xB2\xB2\xB2\xB2",	2, 2},	// ㅁ자
		{ "\xB2\xB2\xB2\xB2",	4, 1},	// 1자
		{ "\xB2\xB2\xB2 \xB2 ", 2, 3},	// ㄱ자 반대
		{ " \xB2\xB2\xB2\xB2 ", 2, 3},	// 번개1
		{ " \xB2\xB2\xB2 \xB2", 2, 3},	// ㅓ모양
		{ "\xB2 \xB2\xB2 \xB2", 2, 3}	// 번개2
	};

	BlockShape* blockShape;


	int deltaTime;
	int deltaTimeC;
	void coolTimeManager(int& index);

	int blockDownTimeSet;

	int coolTimeS1;
	int coolTimeS2;
	int coolTimeS3;
	int skill1_stopBlock;
	void skill2_breakThreeBlock();

	bool guideLineSwich;
public:
	BlockScript(GameObject* gameObject);
	~BlockScript();

	void start();
	void update();

	void SetblockDownTime();

	void guideLine();
	bool isGuideLine() { return guideLineSwich; }

	void move();
	void playerskill();

	int getSkillCoolTime(int skillNum);

	bool isPlace();
	bool isLCollision();
	bool isRCollision();
	void eraseFullLine();

	void CopyShape(BlockScript* copy);

	void rotateShape(Vector2& pos); 
	
};



//
//
//class Block : public GameObject {
//	float x;
//	float y;
//	float speed;
//	bool interactable;
//	Map* map;
//
//public:
//	Block(const BlockShape& shape, bool interactable = true, const Position& pos = Position{ 0, 0 })
//		: GameObject(shape.shape.c_str(), shape.width, shape.height, pos), x(.0f), y(.0f), speed(0.02f), interactable(interactable), map(nullptr)
//	{ }
//
//	void rotateShape() {
//		static char* shape = new char[GameObject::width*GameObject::height];
//		for (int y = 0; y < height; y++)
//			for (int x = 0; x < width; x++)
//				shape[(GameObject::width - 1 - x)*GameObject::height + y] = this->shape[y*GameObject::width + x];
//		setShape(shape);
//		swap(GameObject::width, GameObject::height);
//	}
//
//	void setInteractable() { interactable = true; }
//
//	void setPos(int x, int y) {
//		GameObject::setPos(x, y);
//	}
//
//	void setMap(Map* map) {
//		this->map = map;
//	}
//
//	void update() {
//		static vector<int> comboBonus{ 0, 40, 100, 300, 1200 };
//		if (isActive() == false) return;
//		if (!interactable || !map) return;
//
//		if (map->isGrounded(shape, pos, width, height)) {
//			map->place(shape, pos, width, height);
//			for (int i = height - 1; i >= 0; --i) {
//				int nCombos = 0;
//				while (map->evaluateLine(pos.y + i)) {
//					nCombos++;
//				}
//				if (nCombos < comboBonus.size())
//					map->addScore(comboBonus[nCombos]);
//			}
//			setActive(false);
//			return;
//		}
//
//		if (Input::GetKeyDown(KeyCode::Right)) {
//			x++;
//			if (parent) {
//				if (x + width >= parent->getWidth()) x = parent->getWidth() - width;
//			}
//			else {
//				if (x + width >= screen.getWidth() / 2) x = screen.getWidth() - width;
//			}
//		}
//		if (Input::GetKeyDown(KeyCode::Left)) {
//			x--;
//			if (x < 0.0f) x = 0.0f;
//		}
//		if (Input::GetKeyDown(KeyCode::Up)) {
//			rotateShape();
//		}
//		if (Input::GetKeyDown(KeyCode::Down)) {
//			speed *= 2;
//		}
//		if (Input::GetKeyDown(KeyCode::Space)) {
//			if (map) {
//				pos.y = y;
//				while (!map->isGrounded(shape, pos, width, height)) {
//					pos.y++; y += 1.0f;
//				}
//				return;
//			}
//		}
//		if (Input::GetKeyDown(KeyCode::A)) {
//			rotateShape();
//		}
//		y += speed;
//	}
//};
