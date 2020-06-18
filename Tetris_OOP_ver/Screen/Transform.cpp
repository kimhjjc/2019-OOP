#include "Component.h"
#include "Transform.h"
#include "GameObject.h"

Transform::Transform(GameObject* gameObject,
	const string& shape,
	const Vector2& position,
	const Vector2& rotation,
	const Vector2& scale)
	: position(position),
	rotation(rotation),
	scale(scale),
	shape(shape),
	shapeX(1),
	shapeY(1),
	screen(Screen::getInstance()),
	Component(gameObject)
{
	
}

Transform::~Transform()
{
}

void Transform::start() {}

void Transform::update() 
{
	if (shape == "") return;
	
	//부모 GameObject가 있으면 부모의 Position을 고려하여 Screen에 출력
	GameObject* parent = gameObject->getParent();
	if (parent)
		screen.blockDraw(shape.c_str(), shapeX, shapeY, position + parent->getTransform()->getPosition());
	else
		screen.blockDraw(shape.c_str(), shapeX, shapeY, position);
}
