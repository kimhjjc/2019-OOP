#include "GameEngine.h"
#include "GameObject.h"
#include "MyScript.h"
#include "GridManager.h"
#include <ctime>

GameEngine* GameEngine::instance = nullptr;

GameEngine& GameEngine::getInstance() {
	if (instance == nullptr) {
		instance = new GameEngine();
	}
	return *instance;
}


GameEngine::GameEngine() 
	: screen(Screen::getInstance()) {
	string mode = "mode con cols=" + to_string(screen.getWidth() + 10);
	mode += " lines=" + to_string(screen.getHeight() + 5);

	std::system(mode.c_str());
	std::system("chcp 437");
	screen.clear(); screen.render();
}

void GameEngine::mainLoop() {

	auto& objs = GameObject::gameObjects;

	GameObject* map = new GameObject("map", nullptr, "map", "", Vector2{ 5, 5 });
	GameObject::Add(map);
	map->addComponent<GridManager>();

	GameObject* Panel = new GameObject("panel", nullptr, "nextPanel", "", Vector2{20,5});
	Panel->addComponent<PanelDrawScript>();
	objs.push_back(Panel);

	GameObject* text = new GameObject("text", Panel, "text", "", Vector2{ 0,0 });
	text->addComponent<TextScript>();
	objs.push_back(text);

	GameObject* score = new GameObject("score", Panel, "score", "", Vector2{ 0,0 });
	score->addComponent<ScoreScript>();
	objs.push_back(score);

	GameObject* blockSpawner = new GameObject("blockSpawner", nullptr, "blockSpawner");
	blockSpawner->addComponent<blockSpawnerScript>();
	objs.push_back(blockSpawner);


	for (auto obj : objs)
	{
		obj->traverseStart();
	}

	while (!Input::GetKeyDown(KeyCode::Esc)){
		screen.clear();
		// update
		for (auto obj : objs)
		{
			obj->traverseUpdate();
		}
		// erase in active objects

		// draw

		screen.render();

		Input::EndOfFrame();
	}

	while (objs.size())
	{
		auto obj = objs.back();
		objs.pop_back();
		delete obj;
	}

	

	return;
}