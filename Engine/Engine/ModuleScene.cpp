#include "ModuleScene.h"

#include "Application.h"
#include "InputManager.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "Serializer.h"
#include "FileSystem.h"
#include "ModuleTextures.h"
#include "TimeManager.h"

#include "SphereGenerator.h"
#include "CubeGenerator.h"
#include "CylinderGenerator.h"

// Constructors =================================
ModuleScene::ModuleScene(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
ModuleScene::~ModuleScene()
{

}

// Game Loop ====================================
bool ModuleScene::Start()
{
	//Create the root game object
	root_gameobject = CreateGameObject();
	root_gameobject->SetName("Scene");

	//Set a default octree configuration
	int bound_size = 10;
	math::AABB boundaries = math::AABB(math::float3(-bound_size, -bound_size, -bound_size), math::float3(bound_size, bound_size, bound_size));
	octree.SetBoundaries(boundaries);
	octree.SetMaxObjects(1);
	
	//Set a default game timing configuration
	App->time_manager->SetGameTimeScale(1.0f);

	//Load a scene if theres a saved one
	char str[50];
	sprintf(str, "%sscene.json", LIBRARY_FOLDER);
	if (!App->scene->LoadSerializedScene(str))
	{
		LOG("No default saved scene!");
		//Generate a default main camera
		GameObject* main_camera = CreateGameObject();
		main_camera->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
		main_camera->CreateComponent(COMPONENT_TYPE::COMP_CAMERA);
		main_camera->SetName("Main Camera");
	}
	else LOG("Scene loaded correctly!");

	return true;
}

bool ModuleScene::SceneUpdate(float dt)
{
	bool ret = true;

	//Exit engine
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_STATE::INPUT_DOWN)
	{
		App->SetQuit();
	}
	
	//Update the scene game objects
	float game_dt = App->time_manager->GetGameDT();
	if (scene_update_state == SCENE_UPDATE_STATE::PAUSE_SCENE_STATE)game_dt = 0;
	else if(scene_update_state != EDIT_SCENE_STATE)App->time_manager->AddGameTimeSinceStartup(game_dt);
	ret = root_gameobject->Update(game_dt);

	//Draw the octree
	App->renderer3D->DisableGLRenderFlags();
	octree.Draw();
	App->renderer3D->EnableGLRenderFlags();

	return ret;
}

bool ModuleScene::CleanUp()
{
	RELEASE(root_gameobject);
	
	return true;
}

// Set Methods ==================================
void ModuleScene::SetSelectedGameObject(const GameObject * target)
{
	selected_gameobject = (GameObject*)target;
}

// Get Methods ==================================
GameObject * ModuleScene::GetSelectedGameObject() const
{
	return selected_gameobject;
}

// Functionality ================================
GameObject * ModuleScene::CreateGameObject()
{
	GameObject * obj = new GameObject();
	
	obj->SetParent(root_gameobject);
	
	return obj;
}

bool ModuleScene::RemoveGameObject(GameObject * target, const GameObject * parent, bool search_in)
{
	GameObject* root = (GameObject*)parent;
	if (root == nullptr)root = root_gameobject;

	return root->RemoveChild(target, search_in);
}

GameObject * ModuleScene::FindGameObject(uint id) const
{
	return root_gameobject->FindChild(id);
}

GameObject * ModuleScene::CreatePrimitive(PRIMITIVE_TYPE type, uint divisions)
{
	GameObject* new_prim = nullptr;
	ComponentMesh* mesh = nullptr;
	std::pair<std::vector<uint>, std::vector<Vertex>> data;

	switch (type)
	{
	case UNDEF_PRIMITIVE:
		break;
	case PRIMITIVE_POINT:
		break;
	case PRIMITIVE_LINE:
		break;
	case PRIMITIVE_PLANE:
		break;
	case PRIMITIVE_CUBE:
	{
		//Generate the cube logic
		math::AABB cube_logic;
		cube_logic.minPoint = math::float3(0, 0, 0);
		cube_logic.maxPoint = math::float3(1, 1, 1);
		CubeGenerator cube;
		cube.SetGeometry(cube_logic);
		cube.SetDivisions(divisions);
		data = cube.Generate();

		//Create the game object
		new_prim = CreateGameObject();
		new_prim->SetName("Cube");

		//Create the cube mesh
		ComponentCubeMesh* cube_mesh = (ComponentCubeMesh*)new_prim->CreateComponent(COMPONENT_TYPE::COMP_CUBE_MESH);
		cube_mesh->SetGeometry(cube_logic);
		cube_mesh->SetDivisions(divisions);
		mesh = cube_mesh;
	}
	break;
	case PRIMITIVE_SPHERE:
	{
		//Generate the sphere logic
		math::Sphere sphere_logic;
		sphere_logic.r = 1;
		sphere_logic.pos = math::float3(0, 0, 0);
		SphereGenerator sphere;
		sphere.SetGeometry(sphere_logic);
		sphere.SetDivisions(divisions);
		data = sphere.Generate();
		
		//Create the game object
		new_prim = CreateGameObject();
		new_prim->SetName("Sphere");

		//Create the sphere mesh
		ComponentSphereMesh* sphere_mesh = (ComponentSphereMesh*)new_prim->CreateComponent(COMPONENT_TYPE::COMP_SPHERE_MESH);
		sphere_mesh->SetGeometry(sphere_logic);
		sphere_mesh->SetDivisions(divisions);
		mesh = sphere_mesh;
	}
	break;
	case PRIMITIVE_CYLINDER:
	{
		//Generate the cube logic
		math::Cylinder cylinder_logic;
		cylinder_logic.r = 1;
		cylinder_logic.l.b = math::float3(0, 0, 0);
		cylinder_logic.l.a = math::float3(0, 1, 0);
		CylinderGenerator cylinder;
		cylinder.SetGeometry(cylinder_logic);
		cylinder.SetDivisions(divisions);
		data = cylinder.Generate();

		//Create the game object
		new_prim = CreateGameObject();
		new_prim->SetName("Cylinder");

		//Create the cylinder mesh
		ComponentCylinderMesh* cylinder_mesh = (ComponentCylinderMesh*)new_prim->CreateComponent(COMPONENT_TYPE::COMP_CYLINDER_MESH);
		cylinder_mesh->SetGeometry(cylinder_logic);
		cylinder_mesh->SetDivisions(divisions);
		mesh = cylinder_mesh;
	}
	break;
	case PRIMITIVE_RAY:
		break;
	case PRIMITIVE_CAPSULE:
		break;
	case PRIMITIVE_FRUSTUM:
		break;
	case PRIMITIVE_GRID:
		break;
	default:
		break;
	}

	if (new_prim != nullptr && mesh != nullptr)
	{
		//Generate the game object
		new_prim->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);

		//Primitive mesh renderer
		ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)new_prim->CreateComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);
		mesh_renderer->SetTargetMesh(mesh);

		//Set cube logic in mesh
		mesh->SetIndices(data.first);
		mesh->SetVertices(data.second);
		mesh->SetupMesh();
		new_prim->AdjustBoundingBox();
	}

	return new_prim;
}

void ModuleScene::BlitConfigInfo()
{
	ImGui::Text("Octree Configuration");
	if (ImGui::Button("Recalculate", ImVec2(90, 25)))
	{
		CleanOctree();
		ReFillOctree();
	}
	ImGui::SameLine();
	if (ImGui::Button("Clean", ImVec2(80, 25)))
	{
		CleanOctree();
	}

}

void ModuleScene::BlitHierarchy()
{
	root_gameobject->BlitGameObjectHierarchy(0);
}

bool ModuleScene::GetHierarchyWinState() const
{
	return hierarchy_win_state;
}

void ModuleScene::SwapHierarchyWinState()
{
	hierarchy_win_state = !hierarchy_win_state;
}

void ModuleScene::BlitInspector()
{
	if (selected_gameobject != nullptr)
	{
		selected_gameobject->BlitGameObjectInspector();
	}
}

bool ModuleScene::GetInspectorState() const
{
	return inspector_state;
}

void ModuleScene::EnableInspector()
{
	inspector_state = true;
}

void ModuleScene::DisableInspector()
{
	inspector_state = false;
}

bool ModuleScene::IsRoot(const GameObject * target) const
{
	return (target == root_gameobject);
}

GameObject * ModuleScene::GetRoot() const
{
	return root_gameobject;
}

void ModuleScene::PushGameObjectInOctree(GameObject * target, bool _childs)
{
	//Insert the game object if is static
	if (target->GetStatic())
	{
		octree.Insert(target, *target->GetBoundingBox());
	}

	if (_childs)
	{
		std::vector<GameObject*> childs = *target->GetChilds();
		uint size = childs.size();
		for (uint k = 0; k < size; k++)
		{
			//Insert the childs if are static
			if(childs[k]->GetStatic())PushGameObjectInOctree(childs[k], _childs);
		}
	}
}

void ModuleScene::ReFillOctree()
{
	uint size = static_objects.size();
	for (uint k = 0; k < size; k++)
	{
		octree.Insert(static_objects[k], *static_objects[k]->GetBoundingBox());
	}
}

void ModuleScene::CollectOctreeCandidates(math::Frustum & frustum, std::queue<GameObject*>& queue)
{
	octree.CollectCandidates(frustum, queue);
}

void ModuleScene::CleanOctree()
{
	octree.Reset();
}

void ModuleScene::InsertStaticObject(const GameObject * target)
{
	static_objects.push_back((GameObject*)target);
}

void ModuleScene::RemoveStaticObject(const GameObject * target)
{
	uint size = static_objects.size();
	for (uint k = 0; k < size; k++)
	{
		if (static_objects[k] == target)
		{
			for (uint h = k; h < size - 1; h++)
			{
				static_objects[h] = static_objects[h + 1];
			}

			static_objects.pop_back();

			break;
		}
	}
}

void ModuleScene::HideStaticObjects()
{
	uint size = static_objects.size();
	for (uint k = 0; k < size; k++)
	{
		static_objects[k]->SetActiveState(false);
	}
}

void ModuleScene::SerializeScene() const
{
	//Create the serialized file root
	Serializer serialized_file;

	//Insert GameObjects array
	Serializer game_objects_array(serialized_file.InsertArray("GameObjects"));

	//Here we iterate all the game objects and save all the necessary data
	bool correct = true;
	std::vector<GameObject*> objs = *root_gameobject->GetChilds();
	uint size = objs.size();
	for (uint k = 0; k < size; k++)
	{
		correct = objs[k]->Save(game_objects_array);
		if (!correct)
		{
			LOG("[error] Error Serializing Scene!");
			break;
		}
	}

	if (correct)
	{
		//Save the built json file
		char* buffer = nullptr;
		uint size = serialized_file.Save(&buffer);
		App->fs->SaveFile("scene.json", buffer, size - 1, LIBRARY_FOLDER);

		RELEASE_ARRAY(buffer);
	}
}

bool ModuleScene::LoadSerializedScene(const char * path)
{
	bool ret = true;

	const JSON_Value* scene_data = json_parse_file(path);
	if (scene_data == nullptr)
	{
		LOG("[error] Invalid scene, file not found!");
		return false;
	}

	LOG("Loading Scene...");
	//Serializer whit all the new scene data
	Serializer scene(scene_data);

	//Game objects array
	Serializer objects = scene.GetArray("GameObjects");

	//Iterate all the elements
	uint size = objects.GetArraySize();
	for(uint k = 0; k < size;k++)
	{
		GameObject* new_obj = CreateGameObject();
		new_obj->Load(objects.GetArrayElement(k), objects_links, components_links);
	}

	//Link all the loaded objects
	size = objects_links.size();
	for(uint k = 0; k < size; k++)
	{
		GameObject* parent = FindGameObject(objects_links[k].second);
		if (parent != nullptr)objects_links[k].first->SetParent(parent);
	}
	
	objects_links.clear();

	if (ret)LOG("Scene Correctly Loaded!");

	return ret;
}

bool ModuleScene::InitializeScene()
{
	return root_gameobject->Start();
}

void ModuleScene::CleanScene()
{
	RELEASE(root_gameobject);
	root_gameobject = CreateGameObject();
	root_gameobject->SetName("Scene");
	selected_gameobject = nullptr;
}

void ModuleScene::PlayGame()
{
	if (scene_update_state == PLAY_SCENE_STATE)
	{
		scene_update_state = EDIT_SCENE_STATE;
		App->scene->CleanScene();
		char str[50];
		sprintf(str, "%sscene.json", LIBRARY_FOLDER);
		App->scene->LoadSerializedScene(str);
		LOG("Game Ended!");
		App->time_manager->SetGameTimeSinceStartup(0.0f);
	}
	else
	{
		scene_update_state = PLAY_SCENE_STATE;
		App->scene->SerializeScene();
		App->scene->InitializeScene();
		LOG("Game Started!");
	}
}

void ModuleScene::PauseGame()
{
	if (scene_update_state != PLAY_SCENE_STATE && scene_update_state != PAUSE_SCENE_STATE)return;

	if (scene_update_state == PAUSE_SCENE_STATE)
	{
		scene_update_state = PLAY_SCENE_STATE;
		LOG("Game Unpaused!");
	}
	else
	{
		scene_update_state = PAUSE_SCENE_STATE;
		LOG("Game Paused!");
	}
}

void ModuleScene::NextGameFrame()
{
	scene_update_state = NEXT_SCENE_STATE;
}
