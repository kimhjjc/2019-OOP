#include "GridManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameEngine.h"


GridManager::GridManager(GameObject* gameObject)
	: Component(gameObject), screen(Screen::getInstance()),
	width(10), height(20), gameObjects(GameObject::GetGameObjects()), map(new bool[width * height])
{
}


GridManager::~GridManager()
{
}


void GridManager::start()
{
	for (int i = 0; i < width * height; i++)
		map[i] = false;
}

void GridManager::update()
{
	// map의 배경을 그려줌
	Vector2 m_position = transform->getPosition();
	screen.drawRect(m_position + Vector2{ -1,-1 }, width + 2, height + 2);
	for (int i = 0; i < height; i++)
		screen.backLineDraw(width, height, m_position);

	// map에 쌓인 블록의 정보를 화면에 그려줌
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			if (map[j + i * width])
				screen.draw("\xB2", 1, 1, Vector2{ j,i } + transform->getPosition());


	// 게임이 끝나면 GameOver 화면으로 넘어감
	if (isGameOver())
	{
		GameObject::FindWithTag("nextPanel")->setTag("gameOverPanel");
		for (auto& obj : GameObject::GetGameObjects())
			if ( !(obj == GameObject::FindWithTag("text") || obj == GameObject::FindWithTag("score") ||
				obj == GameObject::FindWithTag("gameOverPanel")) )
				obj->setActive(false);
		
		Sleep(1000);
	}
}

// 첫 라인(가장 윗부분)에 블록이 차면 GameOver
bool GridManager::isGameOver()
{
	for (int i = 0; i < width; i++)
		if (map[i])
			return true;

	return false;
}

// 맵의 한 라인을 지움
void GridManager::mapLineErase(int y)
{
	for (int i = y; i > 1; i--)
		for (int x = 0; x < width; x++)
			map[i*width + x] = map[(i - 1)*width + x];

	for (int x = 0; x < width; x++)
		map[x] = false;
}
// map에 블록을 쌓음
void GridManager::setmap(const char* shape, int w, int h, const Vector2& pos)
{
	if (!shape) return;
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			if (shape[j + i * w] == '\xB2')
				map[pos.X() + j + (pos.Y() + i)*width] = true;
}
