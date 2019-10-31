// Screen.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <array>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include "Utils.h"

using namespace std;

class GameObject {
	bool		enabled;

protected:

	Position	pos;
	Screen&		screen;
	char*		shape;
	int			width;
	int			height;

	vector<GameObject *> children;
	GameObject*	parent;

public:
	GameObject(const char* shape, int width, int height, const Position& pos = Position{ 0, 0 })
		: height(height), width(width), shape(nullptr), pos(pos), enabled(true), parent(nullptr),
		screen(Screen::getInstance()) {
		if (!shape || strlen(shape) == 0)
			this->shape = nullptr;
		else {
			this->shape = new char[width*height];
			strncpy(this->shape, shape, width*height);
		}
		children.clear();
	}

	GameObject(int width, int height, const Position& pos = Position{ 0, 0 })
		: GameObject(nullptr, width, height, pos)
	{}

	virtual ~GameObject()
	{
		while (children.size() > 0) {
			auto back = children.back();
			children.pop_back();
			delete back;
		}
		if (shape) { delete[] shape; }
		shape = nullptr;
		width = 0, height = 0;
	}

	void setParent(GameObject* parent) {
		this->parent = parent;
	}

	void add(GameObject* child) {
		if (!child) return;

		child->setParent(this);
		children.push_back(child);
	}

	virtual bool isActive() { return enabled; }

	void setActive(bool flag = true) { enabled = flag; }

	void setShape(const char* shape) {
		if (!shape) return;
		strncpy(this->shape, shape, width*height);
	}

	virtual void setPos(int x, int y) { this->pos.x = x; this->pos.y = y; }

	int getWidth() const { return width; }
	int getHeight() const { return height; }

	Position& getPos() { return pos; }

	vector<GameObject *>& getChildren() {
		return children;
	}

	void internalDraw(const Position& inheritedPos = Position{ 0, 0 }) {
		if (!enabled) return;

		draw(inheritedPos);

		for (auto& child : children) child->internalDraw(pos + inheritedPos);
	}

	virtual void draw(const Position& inheritedPos = Position{ 0, 0 }) {
		screen.draw(shape, width, height, pos + inheritedPos);
	}

	void internalUpdate() {
		if (!enabled) return;
		update();
		for (auto& child : children) child->internalUpdate();
	}

	virtual void update() {}

	static const char rect = '\xB2';
};


class Panel : public GameObject {

public:
	Panel(const char* layout, int width, int height, const Position& pos) : GameObject(layout, width, height, pos) {}

	GameObject* pop() {
		auto& children = getChildren();
		auto back = children.back();
		children.pop_back();
		back->setParent(nullptr);
		return back;
	}

	void draw(const Position& inheritedPos) {
		screen.drawRect(Position{ pos.x - 1, pos.y - 1 } +inheritedPos, getWidth() + 2, getHeight() + 2);
	}
};

class Text : public Panel {

public:
	Text(const char* info, const Position& pos) : Panel(info, strlen(info), 1, pos) {}

	void draw(const Position& inheritedPos) {
		screen.draw(shape, width, 1, pos + inheritedPos);
	}
};

class Score : public Text {
	int score;
	char* buf;

public:
	Score(const Position& pos) : Text("Score:     ", pos), score(0) {
		buf = new char[strlen(shape) + 1];
	}

	~Score() {
		delete[] buf;
	}

	void addScore(int inc) {
		if (inc < 0) return;
		score += inc;
	}

	void minusScore(int dec) {
		if (dec < 0) return;
		score -= dec;
	}

	void setScore(int index)
	{
		score = index;
	}

	void draw(const Position& inheritedPos) {
		sprintf(buf, "Score: %3d", score);
		screen.draw(buf, strlen(buf), 1, pos + inheritedPos);
	}
};

class Map : public Panel {
	bool* map;
	int   width;
	int	  height;
	int   upper;
	char*  buffer;
	Score* score1;
	Score* score2;

public:
	Map(int width, int height, const Position& pos)
		: map(new bool[width*height]), width(width), height(height), upper(height - 1),
		Panel(nullptr, width, height, pos), score1(nullptr), score2(nullptr), buffer(new char[width*height])
	{
		for (int i = 0; i < width*height; i++)
			map[i] = false;
		memset(buffer, '\xDB', width*height);
	}

	~Map() { if (map) delete[] map; }

	void setScore1(Score* score) {
		this->score1 = score;
	}

	void setScore2(Score* score) {
		this->score2 = score;
		this->score2->setScore(width*height);
	}

	void addScore1(int score) {
		if (this->score1) this->score1->addScore(score);
	}

	void addScore2(int score) {
		if (this->score2) this->score2->addScore(score);
	}

	void update() {
		score1->addScore(1);

		int isBufferNum = 0;
		for (int i = 0; i < width*height; i++)
			if (buffer[i] == '\xDB')
				isBufferNum++;
		score2->setScore(isBufferNum);
	}

	void draw(const Position& inheritedPos)
	{
		screen.drawRect(Position{ pos.x - 1, pos.y - 1 } +inheritedPos, width + 2, height + 2);
		screen.draw(buffer, width, height, Position{ 1, 1 });
	}

	void setBuffer(Position pos) { buffer[pos.x + pos.y*width] = '\xF9'; };
	char getBuffer(Position pos) { return buffer[pos.x + pos.y*width]; };
};

struct BlockShape {
	string shape;
	int width;
	int height;
};

class Monster : public GameObject {

	Map* map;
	Score* score;
public:
	Monster(const char shape, const Position& pos = Position(0, 0))
		:GameObject(&shape, 1, 1, pos)
	{ }

	void setPos(int x, int y) {
		GameObject::setPos(x, y);
	}

	void setMap(Map* map) {
		this->map = map;
	}

	void setScore(Score* score) {
		this->score = score;
	}

	void update() {
		move();
		if (map->getBuffer(pos) == '\xDB')
			score->addScore(1);

		map->setBuffer(pos);
	}

	void move() {

		int indexX = rand() % 3 - 1;
		int indexY = rand() % 3 - 1;

		while ((pos.x + indexX) < 0 || map->getWidth() <= (pos.x + indexX))
			indexX = rand() % 3 - 1;
		while ((pos.y + indexY) < 0 || map->getHeight() <= (pos.y + indexY))
			indexY = rand() % 3 - 1;

		pos.x += indexX;
		pos.y += indexY;
	}

	char getShape() { return *shape; }
};

int main()
{
	Monster a('A');
	Monster b('B');
	Monster c('C');
	Monster d('D');

	Screen&	 screen = Screen::getInstance();
	vector<GameObject *> gameObjects;

	string mode = "mode con cols=" + to_string(screen.getWidth() + 10);
	mode += " lines=" + to_string(screen.getHeight() + 5);

	std::system(mode.c_str());
	std::system("chcp 437");


	auto main = new Map(16, 8, Position{ 1, 1 });
	auto& children = main->getChildren();
	main->add(new Monster(a.getShape(), Position{ 4, 2 }));
	main->add(new Monster(b.getShape(), Position{ 10, 5 }));
	main->add(new Monster(c.getShape(), Position{ 4, 5 }));
	main->add(new Monster(d.getShape(), Position{ 10, 2 }));

	auto panel = new Panel(nullptr, screen.getWidth() / 2 - 2, 15, Position{ screen.getWidth() / 2 + 1, 1 });
	auto scoreA = new Score(Position{ 2,2 });
	auto scoreB = new Score(Position{ 2,4 });
	auto scoreC = new Score(Position{ 2,6 });
	auto scoreD = new Score(Position{ 2,8 });
	auto scoreT = new Score(Position{ 2,11 });
	auto scoreI = new Score(Position{ 2,13 });

	panel->add(new Text("A :", Position{ 1,1 }));
	panel->add(new Text("B :", Position{ 1,3 }));
	panel->add(new Text("C :", Position{ 1,5 }));
	panel->add(new Text("D :", Position{ 1,7 }));
	panel->add(new Text("Total Move Count :", Position{ 1,10 }));
	panel->add(new Text("Remained Item Count :", Position{ 1,12 }));
	panel->add(scoreA);
	panel->add(scoreB);
	panel->add(scoreC);
	panel->add(scoreD);
	panel->add(scoreT);
	panel->add(scoreI);
	static_cast<Monster *>(children[0])->setScore(scoreA);
	static_cast<Monster *>(children[1])->setScore(scoreB);
	static_cast<Monster *>(children[2])->setScore(scoreC);
	static_cast<Monster *>(children[3])->setScore(scoreD);
	main->setScore1(scoreT);
	main->setScore2(scoreI);
	for (int i = 0; i < children.size(); i++)
	{
		static_cast<Monster *>(children[i])->setMap(main);
	}

	gameObjects.push_back(main);
	gameObjects.push_back(panel);
	screen.clear(); screen.render();

	while (!Input::GetKeyDown(KeyCode::Esc)) {
		screen.clear();
		for (auto obj : gameObjects) obj->internalUpdate();

		for (auto it = gameObjects.cbegin();
			it != gameObjects.cend(); it++)
			(*it)->internalDraw();

		screen.render();
		Sleep(1000);		

		Input::EndOfFrame();
	}

	while (gameObjects.size() > 0) {
		auto back = gameObjects.back();
		gameObjects.pop_back();
		delete back;
	}

	return 0;
}