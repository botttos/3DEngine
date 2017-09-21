#include "Application.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleImgui.h"
#include "ModuleConsole.h"
#include "FileSystem.h"
#include "Scene.h"
#include "FileSystem.h"
#include "Parson/parson.h"
#include "imgui/imgui_impl_sdl.h"

// Constructors =================================
Application::Application()
{

	PERF_START(ms_timer);

	window = new ModuleWindow();
	fs = new FileSystem();
	input = new ModuleInput();
	audio = new ModuleAudio();
	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	physics = new ModulePhysics3D();
	imgui = new ModuleImgui();
	console = new ModuleConsole();

	scene = new Scene();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(imgui);
	AddModule(fs);
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	AddModule(console);

	// Scenes
	AddModule(scene);

	AddModule(camera);

	// Renderer last!
	AddModule(renderer3D);

	PERF_PEEK(ms_timer);
}

// Destructors ==================================
Application::~Application()
{
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		RELEASE(*item);
	}
}

// Game Loop ====================================
bool Application::Awake()
{
	PERF_START(ms_timer);

	bool ret = true;

	//Load config json file
	const JSON_Value *config_data = json_parse_file("config.json");
	assert(config_data != NULL);
	const JSON_Object *root_object = json_value_get_object(config_data);
	
	//Load data from config application child
	const JSON_Object* app_object = json_object_dotget_object(root_object, "application");
	app_name = json_object_get_string(app_object, "name");
	organization = json_object_get_string(app_object, "organization");
	max_fps = json_object_get_number(app_object, "max_fps");
	capped_ms = 1000 / (float)max_fps;

	// Call Awake() in all modules
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		const JSON_Object* module_object = json_object_dotget_object(root_object, item._Ptr->_Myval->name.c_str());
		(*item)->Awake(module_object);
	}

	PERF_PEEK(ms_timer);

	return ret;
}

bool Application::Init()
{
	PERF_START(ms_timer);

	bool ret = true;

	// Call Init() in all modules
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret; item++)
	{
		ret = (*item)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret; item++)
	{
		ret = (*item)->Start();
	}

	//Initialize values
	memset(fps_array, 0, 30);

	PERF_PEEK(ms_timer);
	ms_timer.Start();	

	return ret;
}

void Application::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = (float)ms_timer.ReadSec();
	ms_timer.Start();
	frame_time.Start();

	//Generate the imgui frame
	ImGui_ImplSdl_NewFrame(App->window->window);
}

void Application::FinishUpdate()
{
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		Timer t;
		SDL_Delay(capped_ms - last_frame_ms);
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{

	update_status ret = UPDATE_CONTINUE;

	//Start frame timer & ImGui new frame
	PrepareUpdate();
	
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		ret = (*item)->PreUpdate(dt);
	}

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		ret = (*item)->Update(dt);
	}

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		ret = (*item)->PostUpdate(dt);
	}

	FinishUpdate();

	if (want_to_quit)ret = update_status::UPDATE_STOP;

	return ret;
}

bool Application::CleanUp()
{
	PERF_START(ms_timer);

	bool ret = true;

	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		(*item)->CleanUp();
	}

	PERF_PEEK(ms_timer);

	return ret;
}

// Functionality ================================
void Application::SetQuit()
{
	want_to_quit = true;
}

void Application::RequestBrowser(const char* link)
{
	ShellExecute(NULL, "open", link, NULL, NULL, SW_SHOWNORMAL);
}

void Application::ShowConfiguration()
{
	show_config_window = !show_config_window;
}

void Application::BlitConfigWindow()
{
	//Build configuration base window
	ImGui::SetNextWindowPos(ImVec2(900, 100));
	ImGui::SetNextWindowSize(ImVec2(350, 600));
	ImGui::Begin("Configuration", &show_config_window, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);
	//Build application header
	if (ImGui::CollapsingHeader("Application"))
	{
		ImGui::InputText("Title", (char*)app_name.c_str(), 20, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::InputText("Organization", (char*)organization.c_str(), 20);
		ImGui::SliderInt("Max FPS", &max_fps, 0, 120);
		
		//Update framerate graphic
		for (uint k = 0; k < GRAPH_ARRAY_SIZE; k++)
		{
			fps_array[k] = fps_array[k + 1];
		}
		fps_array[GRAPH_ARRAY_SIZE - 1] = ImGui::GetIO().Framerate;
		//Blit framerate graphic
		char fps_title[25];
		sprintf_s(fps_title, 25, "Framerate %.1f", fps_array[29]);
		ImGui::PlotHistogram("Lines", fps_array, IM_ARRAYSIZE(fps_array), 30, fps_title, 0.0f,120.0f, ImVec2(0, 80));
		
		//Update framerate graphic
		for (uint k = 0; k < GRAPH_ARRAY_SIZE; k++)
		{
			miliseconds_array[k] = miliseconds_array[k + 1];
		}
		miliseconds_array[GRAPH_ARRAY_SIZE - 1] = dt * 1000;
		//Blit milliseconds graphic
		char mili_title[25];
		sprintf_s(mili_title, 25, "Milliseconds %.1f", miliseconds_array[29]);
		ImGui::PlotHistogram("Lines", miliseconds_array, IM_ARRAYSIZE(miliseconds_array), 30, mili_title, 0.0f, 100.0f, ImVec2(0, 80));

		if (ImGui::Button("Apply##1", ImVec2(50, 20)))
		{
			//Load config json file
			const JSON_Value *config_data = fs->LoadJSONFile("config.json");
			assert(config_data != NULL);
			//Save the new variable
			json_object_set_string(fs->AccessObject(config_data, 1, "application"), "name", app_name.c_str());
			json_object_set_string(fs->AccessObject(config_data, 1, "application"), "organization", organization.c_str());
			json_object_set_number(fs->AccessObject(config_data, 1, "application"), "max_fps", max_fps);
			//Save the file
			fs->SaveJSONFile(config_data, "config.json");
			//Update window title
			App->window->SetTitle(app_name.c_str());
			capped_ms = 1000 / (float)max_fps;
		}
	}

	//Build headers for the rest of modules
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		if (ImGui::CollapsingHeader((*item)->name.c_str()))
		{
			(*item)->BlitConfigInfo();
		}
	}
	ImGui::End();
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

