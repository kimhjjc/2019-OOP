// Screen.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <ctime>
#include "Utils.h"

using namespace std;

int blockCount = NULL;
int nextBlockCount = NULL;


vector<string> sprites1{ "+\xDB\xDB++\xDB++\xDB", // 첫 출력이 ㄱ자 모양
	"+++++\xDB\xDB\xDB\xDB",
	"+\xDB++\xDB++\xDB\xDB",
	"+++\xDB\xDB\xDB\xDB++"
};

vector<string> sprites2{ "+\xDB\xDB+\xDB++\xDB+", // 첫 출력이 ㄴ자 모양
	"+++\xDB++\xDB\xDB\xDB",
	"++\xDB++\xDB+\xDB\xDB",
	"+++\xDB\xDB\xDB++\xDB"
};

vector<string> sprites3{ "+++\xDB\xDB++\xDB\xDB", // 번개 모양
	"++\xDB+\xDB\xDB+\xDB+",
};

vector<string> sprites4{ "++++\xDB\xDB\xDB\xDB++", // 반대 번개 모양
	"+\xDB++\xDB\xDB++\xDB",
};

vector<string> sprites5{ "++++\xDB+\xDB\xDB\xDB", // ㅗ 모양
	"\xDB++\xDB\xDB+\xDB++",
	"+++\xDB\xDB\xDB+\xDB+",
	"++\xDB+\xDB\xDB++\xDB"
};

vector<string> sprites6{ "\xDB\xDB\xDB\xDB" }; // ㅁ 모양 2 byte

vector<string> sprites7{ "+++\xDB+++\xDB+++\xDB+++\xDB", // 일자 모양 4 byte
	"++++++++++++\xDB\xDB\xDB\xDB",
};

class GameMap {

	int width;
	int height;
	int nextWidth;
	int nextHeight;

	int breakBlockCount;
	int score;

	char *map;
	char *drawMap;
	char *nextBlock;

	bool lineCheck;

	static GameMap* instance;

	Screen &screen;

	bool life;
public:
	static GameMap& getInstance() {
		if (instance == nullptr) {
			instance = new GameMap();
		}
		return *instance;
	}

	GameMap(int width = 10, int height = 20)
		:width(width), height(height), nextWidth(NULL), nextHeight(NULL),
		breakBlockCount(NULL), score(NULL), map(nullptr), drawMap(nullptr),
		nextBlock(nullptr), lineCheck(false), life(true),
		screen(Screen::getInstance()) {
		map = new char[width * height];
		drawMap = new char[width * height];
		nextBlock = new char[4 * 4];
		memset(map, '+', width*height);
		memset(nextBlock, ' ', 4 * 4);
	}

	~GameMap() {
		if (map) { delete[] map; }
		if (drawMap) { delete[] drawMap; }
		width = 0, height = 0;
	}

	const char getMapLChar(int posx, int posy) {
		if (posx % width == -1)
			return '\xDB';
		return drawMap[posx + (posy * width)];
	}

	const char getMapRChar(int posx, int posy) {
		if (posx % width == 0)
			return '\xDB';
		return drawMap[posx + (posy * width)];
	}

	const char getMapUChar(int posx, int posy) {
		if (posy > height - 1)
			return '\xDB';
		return drawMap[posx + (posy * width)];
	}

	void lineFull() {
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				lineCheck = false;
				if (map[j + i * width] == '+') break;
				if (j == (width - 1)) lineCheck = true;
			}
			if (lineCheck)
			{
				char* temp = new char[i*width];
				strncpy(temp, map, i* width);
				strncpy(&map[width], temp, i* width);
				memset(map, '+', width);
				delete[] temp;

				breakBlockCount++;
				score += 100;
			}
		}
	}

	bool lifeCheck() { return life; };
	void setLife() { life = !life; }

	void setMap(const char* shape, int w, int h, const Position& pos)
	{
		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
				if (shape[j + w * i] == '\xDB')
					map[(pos.x - 1) + ((pos.y - 1)*width) + j + width * i] = shape[j + w * i];

		lineFull();
	}

	void setDrawMap(const char* shape, int w, int h, const Position& pos)
	{
		strcpy(drawMap, map);
		if (!shape) return;
		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
				if (shape[j + w * i] == '\xDB')
					drawMap[(pos.x - 1) + ((pos.y - 1)*width) + j + width * i] = shape[j + w * i];
	}

	void setNextBlock(const char* shape, int w, int h)
	{
		nextWidth = w;
		nextHeight = h;

		memset(nextBlock, ' ', 4 * 4);
		if (!shape) return;
		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
				if (shape[j + w * i] == '\xDB')
					nextBlock[j + w * i] = shape[j + w * i];
	}

	void nextBlockDraw() { screen.draw(nextBlock, nextWidth, nextHeight, Position(24, 3)); }

	void clear() { memset(drawMap, '+', width * height); }
	void draw() { screen.draw(drawMap, width, height, Position(1, 1)); }

	void UIDraw() {
		string UI1{ "Break Block Line : " + to_string(breakBlockCount) };
		string UI2{ "Score            : " + to_string(score) };
		screen.draw("Next Block", 10, 1, Position(21, 1));
		screen.draw(UI1.c_str(), UI1.size(), 1, Position(16, 9));
		screen.draw(UI2.c_str(), UI2.size(), 1, Position(16, 10));
	}

	int& getScore() { return score; }

	int getWidth() { return width; }
	int getHeight() { return height; }
};
GameMap* GameMap::instance = nullptr;

class GameObject {
	char*		shape;
	int			width;
	int			height;
	Position	pos;

	Screen&		screen;
	GameMap&    map;

	vector<GameObject *> children;

public:
	GameObject(const char* shape, int width, int height, const Position& pos = Position{ 0, 0 })
		: height(height), width(width),
		shape(nullptr), pos(pos),
		screen(Screen::getInstance()), map(GameMap::getInstance()) {
		if (!shape || strlen(shape) == 0 || width == 0 || height == 0)
		{
			this->shape = new char[1];
			this->shape[0] = 'x';
			width = 1;
			height = 1;
		}
		else {
			this->shape = new char[width*height];
			strncpy(this->shape, shape, width*height);
		}
		this->width = width;
		this->height = height;
	}

	virtual ~GameObject() {
		if (shape) { delete[] shape; }
		width = 0, height = 0;
	}

	void add(GameObject* child) {
		if (!child) return;
		children.push_back(child);
	}

	void setShape(const char* shape) {
		if (!shape) return;
		strncpy(this->shape, shape, width*height);
	}

	void setPos(int x, int y) { this->pos.x = x; this->pos.y = y; }

	Position& getPos() { return pos; }

	int getWidth() { return width; };
	int getHeight() { return height; };

	void internalDraw(const Position& accumulatedPos = Position{ 0,0 })
	{
		draw();
		for (auto child : children)
			child->internalDraw(pos + accumulatedPos);
	}

	virtual void draw()
	{
		map.UIDraw();
		map.draw();
		map.nextBlockDraw();
	}

	void internalUpdate()
	{
		update();
		for (auto child : children)
			child->internalUpdate();
	}

	virtual void update() {}

	virtual void nextBlock() {}

	vector<GameObject *>& getChildren() { return children; }
};

class Block : public GameObject {

	vector<string> sprites;
	int current;
	int nextBlockShape;

	bool leftCheck;
	bool rightCheck;
	bool underCheck;
	bool twoUnderCheck;
	int timer;
	Screen& screen;
	GameMap& map;

	void leftBlockCollision() {
		leftCheck = false;

		for (int i = 0; i < getHeight(); i++)
			for (int j = 0; j < getWidth(); j++)
				if (sprites[current].c_str()[j + i * getWidth()] == '\xDB')
				{
					if (map.getMapLChar(getPos().x + j - 2, getPos().y + i - 1) == '\xDB') return;
					break;
				}
		leftCheck = true;
	}

	void rightBlockCollision() {
		rightCheck = false;

		for (int i = 0; i < getHeight(); i++)
			for (int j = getWidth() - 1; j >= 0; j--)
				if (sprites[current].c_str()[j + i * getWidth()] == '\xDB')
				{
					if (map.getMapRChar(getPos().x + j, getPos().y + i - 1) == '\xDB') return;
					break;
				}
		rightCheck = true;
	}

	void UnderBlockCollision() {
		underCheck = false;
		twoUnderCheck = false;
		for (int j = 0; j < getWidth(); j++)
			for (int i = getHeight() - 1; i >= 0; i--)
				if (sprites[current].c_str()[j + i * getWidth()] == '\xDB')
				{
					if (map.getMapUChar(getPos().x + j - 1, getPos().y + i) == '\xDB') return;
					break;
				}
		underCheck = true;

		for (int j = 0; j < getWidth(); j++)
			for (int i = getHeight() - 1; i >= 0; i--)
				if (sprites[current].c_str()[j + i * getWidth()] == '\xDB')
				{
					if (map.getMapUChar(getPos().x + j - 1, getPos().y + i + 1) == '\xDB') return;
					break;
				}
		twoUnderCheck = true;
	}

	void blockCollision() {leftBlockCollision(); rightBlockCollision(); UnderBlockCollision(); }

	bool rotationBlock()
	{
		for (int j = 0; j < getWidth(); j++)
			for (int i = getHeight() - 1; i >= 0; i--)
				if (sprites[(current + 1) % sprites.size()].c_str()[j + i * getWidth()] == '\xDB'
					&& sprites[current].c_str()[j + i * getWidth()] != '\xDB')
					if (map.getMapLChar(getPos().x + j - 1, getPos().y + i - 1) == '\xDB') return false;
		return true;
	}

public:
	Block(vector<string>& sprites, int w, int h,
		const Position& pos = Position{ 0,0 })
		: sprites(sprites), current(0), leftCheck(false), rightCheck(false),
		underCheck(false), twoUnderCheck(false), timer(5), nextBlockShape(NULL),
		GameObject(sprites[current].c_str(), w, h, pos), screen(Screen::getInstance()), map(GameMap::getInstance()) 
	{
		nextBlockShape = rand() % sprites.size();
		nextBlockCount = rand() % 7;
	}

	void nextBlock() { map.setNextBlock(sprites[current].c_str(), getWidth(), getHeight());	}

	void update() {
		timer--;

		map.clear();
		map.setDrawMap(sprites[current].c_str(), getWidth(), getHeight(), getPos());		

		blockCollision();

		if (Input::GetKeyDown(KeyCode::Right)) {
			if (rightCheck)
				getPos().x++;
		}
		if (Input::GetKeyDown(KeyCode::Left)) {
			if (leftCheck)
				getPos().x--;
		}
		if (Input::GetKeyDown(KeyCode::Up)) {
			if(rotationBlock())
			current = (current + 1) % sprites.size();
			setShape(sprites[current].c_str());
		}
		if (Input::GetKeyDown(KeyCode::Down)) {
			if (underCheck && twoUnderCheck)
				getPos().y = (getPos().y + 1) % map.getHeight();
			else
				timer = 0;
		}
		if (Input::GetKeyDown(KeyCode::A)) {
			current = (current + 1) % sprites.size();
			setShape(sprites[current].c_str());
		}

		if (timer <= 0)
		{
			if (underCheck)
				getPos().y = (getPos().y + 1) % map.getHeight();
			else
			{
				map.setMap(sprites[current].c_str(), getWidth(), getHeight(), getPos());
				map.getScore() += 10;
				if(getPos().y < 3)
				{
					getPos().y = (getPos().y + 1) % map.getHeight();
					map.setDrawMap(sprites[current].c_str(), getWidth(), getHeight(), getPos());
					map.setLife();

					Borland::gotoxy(40, 20);
					printf("!!!!!!!!! \xB1o\xB1 !!!!!!!!!");
					Sleep(2000);
					return;
				}
				current = nextBlockShape;
				blockCount = nextBlockCount;
				nextBlockShape = rand() % sprites.size();
				nextBlockCount = rand() % 7;

				getPos().x = 3;
				getPos().y = 0;
			}
			timer = 5;
		}
	}

};

int main()
{
	GameMap& map = GameMap::getInstance();
	Screen&	 screen = Screen::getInstance();
	vector<GameObject *> gameObjects;

	srand(time(nullptr));

	string mode{ "mode con cols="
		+ to_string(screen.getWidth() + 4)
		+ " lines=" + to_string(screen.getHeight() + 5) };
	system(mode.c_str());
	system("chcp 437");

	auto block1 = new Block{ sprites1, 3,3, Position{4, 0} };
	auto block2 = new Block{ sprites2, 3,3, Position{4, 0} };
	auto block3 = new Block{ sprites3, 3,3, Position{4, 0} };
	auto block4 = new Block{ sprites4, 3,3, Position{4, 0} };
	auto block5 = new Block{ sprites5, 3,3, Position{4, 0} };
	auto block6 = new Block{ sprites6, 2,2, Position{5, 0} };
	auto block7 = new Block{ sprites7, 4,4, Position{2, 0} };

	gameObjects.push_back(block1);
	gameObjects.push_back(block2);
	gameObjects.push_back(block3);
	gameObjects.push_back(block4);
	gameObjects.push_back(block5);
	gameObjects.push_back(block6);
	gameObjects.push_back(block7);

	blockCount = rand() % 7;

	screen.clear(); screen.render();
	while (true)
	{
		screen.clear();
		screen.drawRect(Position(0, 0), 12, 22);
		screen.drawRect(Position(19, 0), 13, 8);

		gameObjects[blockCount]->internalUpdate();
		gameObjects[nextBlockCount]->nextBlock();
		gameObjects[blockCount]->internalDraw();

		if (!map.lifeCheck()) break;

		screen.render();
		Sleep(30);
		Input::EndOfFrame();

	}

	Borland::gotoxy(40, 21);
	printf("GAME OVER\n");
	Sleep(10000);

	return 0;
}