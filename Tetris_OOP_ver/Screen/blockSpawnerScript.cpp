#include "blockSpawnerScript.h"
#include "BlockScript.h"
#include "GameObject.h"
#include "Transform.h"
#include "Utils.h"

using namespace std;

blockSpawnerScript::blockSpawnerScript(GameObject* gameObject)
	: Component(gameObject), nextBlock(nullptr)
{

}


blockSpawnerScript::~blockSpawnerScript()
{
}

void blockSpawnerScript::start()
{
	GameObject* map = GameObject::Find("map");

	// map에서 실제 조작하는 블록
	curruntBlock = new GameObject("block", map, "block");
	curruntBlock->addComponent<BlockScript>();
	GameObject::GameObjects_push_back(curruntBlock);

	// nextPanel에서 출력되는 다음에 나올 블록의 정보
	nextBlock = new GameObject("nextBlock", GameObject::FindWithTag("nextPanel"), "nextBlock");
	nextBlock->addComponent<BlockScript>();
	GameObject::GameObjects_push_back(nextBlock);
}

void blockSpawnerScript::update()
{
	if (curruntBlock->isEnable()) return; 

	// 블록이 맵의 블록 또는 바닥에 충돌 했을 때
	// nextBlock의 값을 currentBlock의 값으로 대입
	BlockScript* curruntBlockC = static_cast<BlockScript *>(curruntBlock->getComponent<BlockScript>());
	BlockScript* nextBlockC = static_cast<BlockScript *>(nextBlock->getComponent<BlockScript>());
	curruntBlockC->CopyShape(nextBlockC);
	nextBlockC->start();

	curruntBlock->setEnable(true);
}