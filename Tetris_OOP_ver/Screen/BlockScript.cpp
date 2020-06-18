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

	// 대입하기 전에 값이 있다면 반환
	if (blockShape)
		delete blockShape;

	static int realrand = 0; // 진짜 랜덤처럼 비슷하게 해주기위해 사용
	srand(time(NULL) + realrand);
	realrand += 100;

	blockDownTimeSet = 1000; // 블록이 내려오는 속도 조절

	// 블록의 정보 입력
	blockShape = new BlockShape{ candidates[rand() % candidates.size()] };
	transform->setShape(blockShape->shape);
	transform->setShapeX(blockShape->width);
	transform->setShapeY(blockShape->height);
	transform->setPosition(Vector2{ gm->getWidth() / 2 - 1,0 });

	//실제 시간의 측정
	deltaTimeC = clock();
	deltaTime = 0;

	// 스킬 쿨타임 1, 2, 3
	coolTimeS1 = 0;
	coolTimeS2 = 0;
	coolTimeS3 = 0;

	// 가이드 라인
	guideLineSwich = true;
}

void BlockScript::update()
{
	if (gameObject->getName() != "block") return;

	//블록이 내려오는 속도 지정
	SetblockDownTime();

	// 스킬
	playerskill();
	
	// 블록 이동 및 회전
	move();

	// 내려오는 블록의 가이드라인 설정
	guideLine();

	// 실제 시간의 측정 1 / 1000초 단위
	deltaTime = clock() - deltaTimeC;
	deltaTimeC = clock();
}

// 블록의 내려오는 속도값 설정
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

// 가이드 라인
void BlockScript::guideLine()
{
	if (Input::GetKeyDown(KeyCode::R)) guideLineSwich = !guideLineSwich;

	if (!guideLineSwich) return;	// 스위치 On, Off 기능

	Screen& screen = Screen::getInstance();
	Vector2 parentPos = gameObject->getParent()->getTransform()->getPosition();
	for (int i = 0; i < 2; i++)
		for (int j = 1; j < gm->getHeight() - (transform->getPosition().Y()); j++)
		{
			// block과 라인이 겹치면 continue
			if (j < blockShape->height - 1)
				if (blockShape->shape[j* blockShape->width + i * (blockShape->width - 1)] == '\xB2'
					|| blockShape->shape[(j + 1)* blockShape->width + i * (blockShape->width - 1)] == '\xB2')
					continue;

			if (j == blockShape->height - 1)
				if (blockShape->shape[j* blockShape->width + i * (blockShape->width - 1)] == '\xB2')
					continue;
			// map에 블록이 쌓여있는지 비교후 처음 부딪히면 break
			if (gm->getmapChar(transform->getPosition() +
				Vector2{ i* (blockShape->width - 1), j }) == true) break;

			// 실제로 보이는 라인 Draw
			screen.draw("\xB3", 1, 1, transform->getPosition() +
				parentPos + Vector2{ i* (blockShape->width - 1), j });
		}
}

void BlockScript::move()
{
	Vector2 pos = transform->getPosition();

	// 내려오는 속도를 카운트, 0이 되면 블록이 한칸 내려옴
	// 중간에 블록을 움직이게 해주는 중간 프레임 역할을 해준다.
	static int downCount = blockDownTimeSet;
	coolTimeManager(downCount); // 실제 시간에 맞춰 1초에 1000씩 index값을 감소

	// 1번 스킬의 사용 유무, skill1_stopBlock 값만큼 Block이 멈춰있는다.
	if (skill1_stopBlock > 0)
	{
		downCount = skill1_stopBlock;
		skill1_stopBlock = 0;
	}

	// 블록을 한번에 일직선상으로 내려 꽂는다
	if (Input::GetKeyDown(KeyCode::Space))
	{
		while (true)
		{
			// 맵에 있는 블록에 닿는 순간
			if (isPlace())
			{
				gm->setmap(blockShape->shape.c_str(), blockShape->width, blockShape->height, transform->getPosition());
				eraseFullLine();
				gameObject->setEnable(false);
				score->setScore(10);
				break;
			}
			//맵의 블록에 닿기 전까지 계속 블록이 내려준다.
			pos.y++;
			transform->setPosition(pos);
		}
	}

	// 블록의 회전 (반시계 방향)
	if (Input::GetKeyDown(KeyCode::Up))
	{
		rotateShape(pos);
	}

	// 왼쪽 이동
	if (Input::GetKeyDown(KeyCode::Left))
	{
		if (isLCollision())return;
		pos.x--;
	}
	// 오른쪽 이동
	if (Input::GetKeyDown(KeyCode::Right))
	{
		if (isRCollision()) return;

		pos.x++;
	}
	// 아래 한칸 이동
	if (Input::GetKeyDown(KeyCode::Down))
	{
		downCount = 0;
	}

	// 블록을 한칸 내려준다.
	if (downCount < 0)
	{
		if (isPlace())
		{
			// 맵에 있는 블록에 닿는 순간
			gm->setmap(blockShape->shape.c_str(), blockShape->width, blockShape->height, transform->getPosition());
			eraseFullLine();
			gameObject->setEnable(false);
			score->setScore(10);
		}
		//맵의 블록에 닿기 전까지 계속 블록이 내려준다.
		pos.y++;
		downCount = blockDownTimeSet;
	}

	// 함수 내에서 입력된 값에 따라 블록의 포지션 값 변경
	transform->setPosition(pos);
}

void BlockScript::playerskill()
{
	//조건 : 1000점 이상 //// 3초간 Block이 멈춤, 5초 쿨타임
	if (Input::GetKeyDown(KeyCode::Q) && score->getScore() >= 1000 && coolTimeS1 < 0)
	{
		skill1_stopBlock = 3000; // 지속시간 3초
		coolTimeS1 = 5000; // 쿨타임 5초

		// 다른 스킬 쿨타임과 동시에 쓰지 못하게 최소한의 쿨타임 3초를 준다.
		if (coolTimeS2 < 3000) coolTimeS2 = 3000;
		if (coolTimeS3 < 3000) coolTimeS3 = 3000;
	}
	//조건 : Max콤보를 4(max)콤보 달성 //// 가장 아래 세 줄을 파괴.(줄당100점), 쿨타임 20초
	if (Input::GetKeyDown(KeyCode::W) && score->getMaxCombo() >= 4 && coolTimeS2 < 0)
	{
		skill2_breakThreeBlock(); // 가장 아랫블록라인 세 칸 지우기
		coolTimeS2 = 20000; // 쿨타임 20초

		// 다른 스킬 쿨타임과 동시에 쓰지 못하게 최소한의 쿨타임 3초를 준다.
		if (coolTimeS1 < 3000) coolTimeS1 = 3000;
		if (coolTimeS3 < 3000) coolTimeS3 = 3000;
	}
	//조건 : 2분 이상 게임플레이 //// 맵에 쌓인 모든 블럭을 처리한다.(1000점), 쿨타임 120초
	if (Input::GetKeyDown(KeyCode::E) && score->getPlayTime() > 120 && coolTimeS3 < 0)
	{
		gm->clearmap();
		score->setScore(1000);
		coolTimeS3 = 120000; // 쿨타임 120초

		// 다른 스킬 쿨타임과 동시에 쓰지 못하게 최소한의 쿨타임 3초를 준다.
		if (coolTimeS1 < 3000) coolTimeS1 = 3000;
		if (coolTimeS2 < 3000) coolTimeS2 = 3000;
	}

	// 쿨타임 관리 1초당 1000씩 index 값이 감소
	coolTimeManager(coolTimeS1);
	coolTimeManager(coolTimeS2);
	coolTimeManager(coolTimeS3);
}

// 스킬 쿨타임 정보 반환
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

// 쿨타임 관리, 1초당 1000씩 index 값이 감소
void BlockScript::coolTimeManager(int & index)
{
	if (index >= 0)
		index -= deltaTime;
}

// 가장 아랫블록라인 세 칸 지우기
void BlockScript::skill2_breakThreeBlock()
{
	for (int y = gm->getHeight() - 3; y < gm->getHeight(); y++)
	{
		int i = 0;
		for (int x = 0; x < gm->getWidth(); x++)
			if (gm->getmapChar(Vector2{ x,y }) == true) i++;

		if (i > 0) // 줄에 블록이 하나라도 있을때
		{
			gm->mapLineErase(y);
			score->setScore(100);
		}
	}
}

// 블록이 맵의 블록과 닿았는지 체크
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

// 왼쪽에 블록과 닿는지 체크
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

// 오른쪽에 블록과 닿는지 체크
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

// 한 줄이 꽉찬 라인 지우기
void BlockScript::eraseFullLine()
{
	int combo = 0;
	for (int y = 0; y < gm->getHeight(); y++)
		for (int x = 0; x < gm->getWidth(); x++)
		{
			if (gm->getmapChar(Vector2{ x,y }) == false) break;
			if (x == gm->getWidth() - 1) // 블록 한줄이 모두 차있을때
			{
				gm->mapLineErase(y); // y 번째 줄을 지움 (정확히는 y+1번 째줄)
				combo++;
				score->setScore(100 * combo);
			}
		}
	// 맥스 콤보값 비교 및 대입
	if (score->getMaxCombo() < combo) score->setMaxCombo(combo);

}

// nextBlock에서 currentBlock 으로 넘어올때 사용.
void BlockScript::CopyShape(BlockScript * copy)
{
	*(this->blockShape) = *(copy->blockShape);
	transform->setShape(blockShape->shape);
	transform->setShapeX(blockShape->width);
	transform->setShapeY(blockShape->height);
	transform->setPosition(Vector2{ 4,0 });
}

// 블록 회전
void BlockScript::rotateShape(Vector2& pos)
{
	int width = blockShape->width;
	int height = blockShape->height;

	char* shape = new char[width*height];
	// 회전된 shape모양 지정
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			shape[(width - 1 - x)*height + y] = blockShape->shape[y*width + x];
	// width와 height가 서로 교차
	width = blockShape->height;
	height = blockShape->width;
	Vector2 tempPos{ pos };
	if (pos.x + width > gm->getWidth())
		tempPos.x -= pos.x + width - gm->getWidth();
	// 바뀐 shape를 옆으로 이동시켜 충돌 비교
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

	// 충돌 시 대입없이 값 반환
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			if ((gm->getmapChar(tempPos + Vector2{ x, y }) &&
				blockShape->shape[x + y * width] == '\xB2')
				|| tempPos.x < 0)
			{
				delete shape;
				return;
			}

	// 회전 가능한 경우 값을 대입.
	if (tempPos != pos)
		pos = tempPos;
	blockShape->shape = shape;
	swap(blockShape->width, blockShape->height);
	transform->setShape(blockShape->shape);
	transform->setShapeX(blockShape->width);
	transform->setShapeY(blockShape->height);

	delete shape;
}
