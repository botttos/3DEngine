#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <vector>
#include <string>

#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMeshRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

class Serializer;

class GameObject
{
public:

	GameObject();
	GameObject(const GameObject& cpy);
	~GameObject();

public:

	bool	Update();

private:

	bool						actived = true;
	bool						static_ = false;
	std::string					name = "Unnamed";
	GameObject*					parent = nullptr;
	std::vector<Component*>		components;
	std::vector<GameObject*>	childs;
	bool						draw_bounding_box = false;
	math::AABB					bounding_box;
	uint						id = 0;

public:

	//Set Methods -----------
	void SetActiveState(bool act);
	void SetStatic(bool st);
	void SetName(const char* str);
	void SetParent(GameObject* target);

	//Get Methods -----------
	bool				GetActive()const;
	bool				GetStatic()const;
	float				GetBoundingBoxDiagonalSize()const;
	uint				GetID()const;
	const GameObject*	GetParent() const;
	bool				IsRoot() const;
	bool				IsSelectedObject() const;
	
	//Functionality ---------
	//Components Methods 
	Component*	CreateComponent(COMPONENT_TYPE c_type);
	bool		RemoveComponent(Component* cmp);
	bool		FindComponent(Component* cmp)const;
	Component*	FindComponent(COMPONENT_TYPE type)const;
	Component*	CloneComponent(const Component* target)const;

	//Childs Methods 
	void						AddChild(const GameObject* child);
	bool						RemoveChild(GameObject* child, bool search_in = false);
	bool						PopChild(GameObject* child, bool search_in = false);
	std::vector<GameObject*>*	GetChilds();

	//UI Methods
	void		BlitGameObjectHierarchy(uint index);
	void		BlitGameObjectInspector();

	//Bounding Box Methods
	std::pair<math::float3, math::float3>	AdjustBoundingBox(bool all_childs = true);
	void									DrawBoundingBox();
	math::AABB*								GetBoundingBox();
	math::AABB								GetTransformedBoundingBox();
	void									UpdateBoundingBox();
	std::vector<math::float3>				childs_aabb_points;

	//Save/Load Methods
	bool Save(Serializer& array_root)const;
	bool Load(const JSON_Object* root);

};
#endif // !_GAME_OBJECT_H_
