#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Utils.h"

using namespace std;

class Component;
class Transform;

class GameObject
{
	bool		enabled;
	string		name;
	string		tag;
	GameObject* parent;
	vector<Component *> components;
	vector<GameObject *> children;

	static vector<GameObject *> gameObjects;
	friend class GameEngine;

	Transform* transform;

public:
	GameObject(const string& name, GameObject* parent = nullptr, const string& tag = "",
		const string& shape="", const Vector2& pos=Vector2::zero);
	
	~GameObject();

	void traverseStart();
	void traverseUpdate();

	bool isEnable() { return enabled; }
	void setEnable(bool enabled) { this->enabled = enabled; }

	string getName() const { return name; }
	string getTag() const { return tag; }
	void setName(const char* name) { this->name = name; }
	void setTag(const char* tag) { this->tag = tag; }

	Transform* getTransform() { return transform; }

	static GameObject* Find(const string& path);
	static GameObject* FindWithTag(const string& path);
	static void Add(GameObject* obj);
	static void Remove(GameObject* obj);

	static const vector<GameObject *>& GetGameObjects() { return gameObjects; }
	static void GameObjects_push_back(GameObject* obj) { GameObject::gameObjects.push_back(obj); }

	GameObject* getParent() { return parent; }
	void setParent(GameObject* parent) 
	{this->parent = parent;}

	virtual bool isActive() 
	{ return enabled; }

	void setActive(bool flag = true) 
	{ enabled = flag; }

	template<typename T>
	void addComponent() {		
		T t{ this };
		if (dynamic_cast<Component*>(&t) == nullptr) {
			return;
		}
		components.push_back(new T{ this });
	}

	template<typename T>
	Component* getComponent() {
		for (auto comp : components)
		{
			if (dynamic_cast<T *>(comp) != nullptr) return comp;
		}
		return nullptr;
	}

};

