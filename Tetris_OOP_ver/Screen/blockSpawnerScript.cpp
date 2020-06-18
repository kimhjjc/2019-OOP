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

	// map���� ���� �����ϴ� ���
	curruntBlock = new GameObject("block", map, "block");
	curruntBlock->addComponent<BlockScript>();
	GameObject::GameObjects_push_back(curruntBlock);

	// nextPanel���� ��µǴ� ������ ���� ����� ����
	nextBlock = new GameObject("nextBlock", GameObject::FindWithTag("nextPanel"), "nextBlock");
	nextBlock->addComponent<BlockScript>();
	GameObject::GameObjects_push_back(nextBlock);
}

void blockSpawnerScript::update()
{
	if (curruntBlock->isEnable()) return; 

	// ����� ���� ��� �Ǵ� �ٴڿ� �浹 ���� ��
	// nextBlock�� ���� currentBlock�� ������ ����
	BlockScript* curruntBlockC = static_cast<BlockScript *>(curruntBlock->getComponent<BlockScript>());
	BlockScript* nextBlockC = static_cast<BlockScript *>(nextBlock->getComponent<BlockScript>());
	curruntBlockC->CopyShape(nextBlockC);
	nextBlockC->start();

	curruntBlock->setEnable(true);
}