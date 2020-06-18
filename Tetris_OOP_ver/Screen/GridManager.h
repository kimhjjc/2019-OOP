#pragma once
#include "Component.h"
#include "Utils.h"
#include <vector>

class GridManager :
	public Component
{
	int width;
	int height;
	Screen& screen;

	bool* map;

	const vector<GameObject *>& gameObjects;

public:
	GridManager(GameObject* gameObject);
	~GridManager();

	int getWidth() const { return width; }
	int getHeight() const { return height; }

	void mapLineErase(int y);
	void setmap(const char* shape, int w, int h, const Vector2& pos);
	const bool getmapChar(Vector2 pos) { return map[(int)(pos.x) + (int)pos.y * width]; }

	void clearmap() {for (int i = 0; i < width * height; i++) map[i] = false;}

	bool isGameOver();
protected:
	void start();
	void update();
};

