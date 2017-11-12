#ifndef _RESOURCES_MANAGER_
#define _RESOURCES_MANAGER_

#include "Module.h"
#include "Primitive_.h"
#include <map>

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

class ResourcesManager : public Module
{
public:

	ResourcesManager(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ResourcesManager();

public:

	bool Start();
	bool CleanUp();

private:

	std::map<uint, Resource*> resources;

	//Primitives resources
	ResourceMesh* cube_mesh = nullptr;
	ResourceMesh* sphere_low_mesh = nullptr;
	ResourceMesh* sphere_hi_mesh = nullptr;
	ResourceMesh* cylinder_low_mesh = nullptr;
	ResourceMesh* cylinder_hi_mesh = nullptr;

public:

	//Resource management methods
	Resource*		CreateResource(RESOURCE_TYPE type);
	ResourceMesh*	GetPrimitiveResourceMesh(PRIMITIVE_TYPE type);

	//Import methods
	bool	ImportFile(const char* path, bool put_on_scene);
	uint	CheckAssetsResources();
	bool	CheckIfFileIsImported(const char* path)const;

};
#endif // !_RESOURCES_MANAGER_