#include "GameObject.h"
#include "Component.h"
#include "MyScript.h"

using namespace std;

MyScript::MyScript(GameObject* gameObject)
	: Component(gameObject) 
{}