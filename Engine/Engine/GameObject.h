#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <vector>
#include <string>

#include "ComponentTransform.h"

class GameObject
{
public:

	GameObject();
	GameObject(const GameObject& cpy);
	~GameObject();

public:

	virtual bool	Update();
	bool			UpdateChilds();
private:

	bool						actived = true;
	std::string					name = "Unnamed";
	GameObject*					parent = nullptr;
	std::vector<Component*>		components;
	std::vector<GameObject*>	childs;

public:

	//Set Methods -----------
	void SetActiveState(bool act);
	void SetName(const char* str);
	void SetParent(GameObject* target);

	//Functionality ---------
	Component*	CreateComponent(COMPONENT_TYPE c_type);
	bool		RemoveComponent(Component* cmp);
	bool		FindComponent(Component* cmp)const;
	Component*	CloneComponent(const Component* target)const;

	void		AddChild(const GameObject* child);
	bool		RemoveChild(GameObject* child, bool search_in = false);
	bool		PopChild(GameObject* child, bool search_in = false);

	void		BlitGameObject();

};
#endif // !_GAME_OBJECT_H_
