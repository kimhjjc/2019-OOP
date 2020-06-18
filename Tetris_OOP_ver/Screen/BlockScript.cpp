#include "BlockScript.h"
#include "GameObject.h"
#include "Transform.h"
#include "Utils.h"
#include <time.h>

BlockScript::BlockScript(GameObject* gameObject)
	:Component(gameObject), gm(nullptr), blockShape(nullptr), score(nullptr)
{
	start();
}


BlockScript::~BlockScript()
{
}

void BlockScript::start()
{
	GameObject* map = GameObject::Find("map");
	if (map == nullptr) return;
	gm = static_cast<GridManager *>(map->getComponent<GridManager>());

	score = static_cast<ScoreScript *>(GameObject::Find("score")->getComponent<ScoreScript>());

	// �����ϱ� ���� ���� �ִٸ� ��ȯ
	if (blockShape)
		delete blockShape;

	static int realrand = 0; // ��¥ ����ó�� ����ϰ� ���ֱ����� ���
	srand(time(NULL) + realrand);
	realrand += 100;

	blockDownTimeSet = 1000; // ����� �������� �ӵ� ����

	// ����� ���� �Է�
	blockShape = new BlockShape{ candidates[rand() % candidates.size()] };
	transform->setShape(blockShape->shape);
	transform->setShapeX(blockShape->width);
	transform->setShapeY(blockShape->height);
	transform->setPosition(Vector2{ gm->getWidth() / 2 - 1,0 });

	//���� �ð��� ����
	deltaTimeC = clock();
	deltaTime = 0;

	// ��ų ��Ÿ�� 1, 2, 3
	coolTimeS1 = 0;
	coolTimeS2 = 0;
	coolTimeS3 = 0;

	// ���̵� ����
	guideLineSwich = true;
}

void BlockScript::update()
{
	if (gameObject->getName() != "block") return;

	//����� �������� �ӵ� ����
	SetblockDownTime();

	// ��ų
	playerskill();
	
	// ��� �̵� �� ȸ��
	move();

	// �������� ����� ���̵���� ����
	guideLine();

	// ���� �ð��� ���� 1 / 1000�� ����
	deltaTime = clock() - deltaTimeC;
	deltaTimeC = clock();
}

// ����� �������� �ӵ��� ����
void BlockScript::SetblockDownTime()
{
	if (score->getScore() > 10000)
	{
		blockDownTimeSet = 100;
	}
	else if (score->getScore() > 5000)
	{
		blockDownTimeSet = 170;
	}
	else if (score->getScore() > 4000)
	{
		blockDownTimeSet = 250;
	}
	else if (score->getScore() > 3000)
	{
		blockDownTimeSet = 300;
	}
	else if (score->getScore() > 2000)
	{
		blockDownTimeSet = 450;
	}
	else if (score->getScore() > 1000)
	{
		blockDownTimeSet = 600;
	}
	else if (score->getScore() > 500)
	{
		blockDownTimeSet = 800;
	}
}

// ���̵� ����
void BlockScript::guideLine()
{
	if (Input::GetKeyDown(KeyCode::R)) guideLineSwich = !guideLineSwich;

	if (!guideLineSwich) return;	// ����ġ On, Off ���

	Screen& screen = Screen::getInstance();
	Vector2 parentPos = gameObject->getParent()->getTransform()->getPosition();
	for (int i = 0; i < 2; i++)
		for (int j = 1; j < gm->getHeight() - (transform->getPosition().Y()); j++)
		{
			// block�� ������ ��ġ�� continue
			if (j < blockShape->height - 1)
				if (blockShape->shape[j* blockShape->width + i * (blockShape->width - 1)] == '\xB2'
					|| blockShape->shape[(j + 1)* blockShape->width + i * (blockShape->width - 1)] == '\xB2')
					continue;

			if (j == blockShape->height - 1)
				if (blockShape->shape[j* blockShape->width + i * (blockShape->width - 1)] == '\xB2')
					continue;
			// map�� ����� �׿��ִ��� ���� ó�� �ε����� break
			if (gm->getmapChar(transform->getPosition() +
				Vector2{ i* (blockShape->width - 1), j }) == true) break;

			// ������ ���̴� ���� Draw
			screen.draw("\xB3", 1, 1, transform->getPosition() +
				parentPos + Vector2{ i* (blockShape->width - 1), j });
		}
}

void BlockScript::move()
{
	Vector2 pos = transform->getPosition();

	// �������� �ӵ��� ī��Ʈ, 0�� �Ǹ� ����� ��ĭ ������
	// �߰��� ����� �����̰� ���ִ� �߰� ������ ������ ���ش�.
	static int downCount = blockDownTimeSet;
	coolTimeManager(downCount); // ���� �ð��� ���� 1�ʿ� 1000�� index���� ����

	// 1�� ��ų�� ��� ����, skill1_stopBlock ����ŭ Block�� �����ִ´�.
	if (skill1_stopBlock > 0)
	{
		downCount = skill1_stopBlock;
		skill1_stopBlock = 0;
	}

	// ����� �ѹ��� ������������ ���� �ȴ´�
	if (Input::GetKeyDown(KeyCode::Space))
	{
		while (true)
		{
			// �ʿ� �ִ� ��Ͽ� ��� ����
			if (isPlace())
			{
				gm->setmap(blockShape->shape.c_str(), blockShape->width, blockShape->height, transform->getPosition());
				eraseFullLine();
				gameObject->setEnable(false);
				score->setScore(10);
				break;
			}
			//���� ��Ͽ� ��� ������ ��� ����� �����ش�.
			pos.y++;
			transform->setPosition(pos);
		}
	}

	// ����� ȸ�� (�ݽð� ����)
	if (Input::GetKeyDown(KeyCode::Up))
	{
		rotateShape(pos);
	}

	// ���� �̵�
	if (Input::GetKeyDown(KeyCode::Left))
	{
		if (isLCollision())return;
		pos.x--;
	}
	// ������ �̵�
	if (Input::GetKeyDown(KeyCode::Right))
	{
		if (isRCollision()) return;

		pos.x++;
	}
	// �Ʒ� ��ĭ �̵�
	if (Input::GetKeyDown(KeyCode::Down))
	{
		downCount = 0;
	}

	// ����� ��ĭ �����ش�.
	if (downCount < 0)
	{
		if (isPlace())
		{
			// �ʿ� �ִ� ��Ͽ� ��� ����
			gm->setmap(blockShape->shape.c_str(), blockShape->width, blockShape->height, transform->getPosition());
			eraseFullLine();
			gameObject->setEnable(false);
			score->setScore(10);
		}
		//���� ��Ͽ� ��� ������ ��� ����� �����ش�.
		pos.y++;
		downCount = blockDownTimeSet;
	}

	// �Լ� ������ �Էµ� ���� ���� ����� ������ �� ����
	transform->setPosition(pos);
}

void BlockScript::playerskill()
{
	//���� : 1000�� �̻� //// 3�ʰ� Block�� ����, 5�� ��Ÿ��
	if (Input::GetKeyDown(KeyCode::Q) && score->getScore() >= 1000 && coolTimeS1 < 0)
	{
		skill1_stopBlock = 3000; // ���ӽð� 3��
		coolTimeS1 = 5000; // ��Ÿ�� 5��

		// �ٸ� ��ų ��Ÿ�Ӱ� ���ÿ� ���� ���ϰ� �ּ����� ��Ÿ�� 3�ʸ� �ش�.
		if (coolTimeS2 < 3000) coolTimeS2 = 3000;
		if (coolTimeS3 < 3000) coolTimeS3 = 3000;
	}
	//���� : Max�޺��� 4(max)�޺� �޼� //// ���� �Ʒ� �� ���� �ı�.(�ٴ�100��), ��Ÿ�� 20��
	if (Input::GetKeyDown(KeyCode::W) && score->getMaxCombo() >= 4 && coolTimeS2 < 0)
	{
		skill2_breakThreeBlock(); // ���� �Ʒ���϶��� �� ĭ �����
		coolTimeS2 = 20000; // ��Ÿ�� 20��

		// �ٸ� ��ų ��Ÿ�Ӱ� ���ÿ� ���� ���ϰ� �ּ����� ��Ÿ�� 3�ʸ� �ش�.
		if (coolTimeS1 < 3000) coolTimeS1 = 3000;
		if (coolTimeS3 < 3000) coolTimeS3 = 3000;
	}
	//���� : 2�� �̻� �����÷��� //// �ʿ� ���� ��� ���� ó���Ѵ�.(1000��), ��Ÿ�� 120��
	if (Input::GetKeyDown(KeyCode::E) && score->getPlayTime() > 120 && coolTimeS3 < 0)
	{
		gm->clearmap();
		score->setScore(1000);
		coolTimeS3 = 120000; // ��Ÿ�� 120��

		// �ٸ� ��ų ��Ÿ�Ӱ� ���ÿ� ���� ���ϰ� �ּ����� ��Ÿ�� 3�ʸ� �ش�.
		if (coolTimeS1 < 3000) coolTimeS1 = 3000;
		if (coolTimeS2 < 3000) coolTimeS2 = 3000;
	}

	// ��Ÿ�� ���� 1�ʴ� 1000�� index ���� ����
	coolTimeManager(coolTimeS1);
	coolTimeManager(coolTimeS2);
	coolTimeManager(coolTimeS3);
}

// ��ų ��Ÿ�� ���� ��ȯ
int BlockScript::getSkillCoolTime(int skillNum)
{
	if (skillNum == 1)
		return coolTimeS1 / 1000;
	if (skillNum == 2)
		return coolTimeS2 / 1000;
	if (skillNum == 3)
		return coolTimeS3 / 1000;

	return 0;
}

// ��Ÿ�� ����, 1�ʴ� 1000�� index ���� ����
void BlockScript::coolTimeManager(int & index)
{
	if (index >= 0)
		index -= deltaTime;
}

// ���� �Ʒ���϶��� �� ĭ �����
void BlockScript::skill2_breakThreeBlock()
{
	for (int y = gm->getHeight() - 3; y < gm->getHeight(); y++)
	{
		int i = 0;
		for (int x = 0; x < gm->getWidth(); x++)
			if (gm->getmapChar(Vector2{ x,y }) == true) i++;

		if (i > 0) // �ٿ� ����� �ϳ��� ������
		{
			gm->mapLineErase(y);
			score->setScore(100);
		}
	}
}

// ����� ���� ��ϰ� ��Ҵ��� üũ
bool BlockScript::isPlace()
{
	if (transform->getPosition().y + blockShape->height > gm->getHeight() - 1) return true;

	for (int i = 0; i < blockShape->height; i++)
		for (int j = 0; j < blockShape->width; j++)
			if (blockShape->shape.c_str()[j + i * blockShape->width] == '\xB2') {
				Vector2 pos = transform->getPosition();
				pos.x += j;
				pos.y += i + 1;
				if (gm->getmapChar(pos) == true) return true;
			}
	return false;
}

// ���ʿ� ��ϰ� ����� üũ
bool BlockScript::isLCollision()
{
	if (transform->getPosition().x <= 0) return true;

	for (int i = 0; i < blockShape->height; i++)
		for (int j = 0; j < blockShape->width; j++)
			if (blockShape->shape.c_str()[j + i * blockShape->width] == '\xB2') {
				Vector2 pos = transform->getPosition();
				pos.x += j - 1;
				pos.y += i;
				if (gm->getmapChar(pos) == true) return true;
			}
	return false;
}

// �����ʿ� ��ϰ� ����� üũ
bool BlockScript::isRCollision()
{
	if (transform->getPosition().x + blockShape->width > gm->getWidth() - 1) return true;

	for (int i = 0; i < blockShape->height; i++)
		for (int j = 0; j < blockShape->width; j++)
			if (blockShape->shape.c_str()[j + i * blockShape->width] == '\xB2') {
				Vector2 pos = transform->getPosition();
				pos.x += j + 1;
				pos.y += i;
				if (gm->getmapChar(pos) == true) return true;
			}
	return false;
}

// �� ���� ���� ���� �����
void BlockScript::eraseFullLine()
{
	int combo = 0;
	for (int y = 0; y < gm->getHeight(); y++)
		for (int x = 0; x < gm->getWidth(); x++)
		{
			if (gm->getmapChar(Vector2{ x,y }) == false) break;
			if (x == gm->getWidth() - 1) // ��� ������ ��� ��������
			{
				gm->mapLineErase(y); // y ��° ���� ���� (��Ȯ���� y+1�� °��)
				combo++;
				score->setScore(100 * combo);
			}
		}
	// �ƽ� �޺��� �� �� ����
	if (score->getMaxCombo() < combo) score->setMaxCombo(combo);

}

// nextBlock���� currentBlock ���� �Ѿ�ö� ���.
void BlockScript::CopyShape(BlockScript * copy)
{
	*(this->blockShape) = *(copy->blockShape);
	transform->setShape(blockShape->shape);
	transform->setShapeX(blockShape->width);
	transform->setShapeY(blockShape->height);
	transform->setPosition(Vector2{ 4,0 });
}

// ��� ȸ��
void BlockScript::rotateShape(Vector2& pos)
{
	int width = blockShape->width;
	int height = blockShape->height;

	char* shape = new char[width*height];
	// ȸ���� shape��� ����
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			shape[(width - 1 - x)*height + y] = blockShape->shape[y*width + x];
	// width�� height�� ���� ����
	width = blockShape->height;
	height = blockShape->width;
	Vector2 tempPos{ pos };
	if (pos.x + width > gm->getWidth())
		tempPos.x -= pos.x + width - gm->getWidth();
	// �ٲ� shape�� ������ �̵����� �浹 ��
	for (int x = 0; x < width; x++) 
		for (int y = 0; y < height; y++)
		{
			if (gm->getmapChar(tempPos + Vector2{ x, y })
				&& shape[x + y * width] == '\xB2')
			{
				tempPos.x -= width - x;
				x = width;
				break;
			}
		}

	// �浹 �� ���Ծ��� �� ��ȯ
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			if ((gm->getmapChar(tempPos + Vector2{ x, y }) &&
				blockShape->shape[x + y * width] == '\xB2')
				|| tempPos.x < 0)
			{
				delete shape;
				return;
			}

	// ȸ�� ������ ��� ���� ����.
	if (tempPos != pos)
		pos = tempPos;
	blockShape->shape = shape;
	swap(blockShape->width, blockShape->height);
	transform->setShape(blockShape->shape);
	transform->setShapeX(blockShape->width);
	transform->setShapeY(blockShape->height);

	delete shape;
}
