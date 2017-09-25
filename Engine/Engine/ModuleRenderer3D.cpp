#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "FileSystem.h"
#include "ModuleAudio.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Engine/Glew/lib/Win32/glew32.lib")

// Constructors =================================
ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	name = "Renderer";
}

// Game Loop ====================================
bool ModuleRenderer3D::Awake(const JSON_Object * data_root)
{
	vsync = json_object_get_boolean(data_root, "vsync");

	depth_test = json_object_get_boolean(data_root, "depth");
	cull_face = json_object_get_boolean(data_root, "cull");
	texture_2d = json_object_get_boolean(data_root, "texture2d");
	lighting = json_object_get_boolean(data_root, "lighting");
	color_material = json_object_get_boolean(data_root, "color_material");
	dither = json_object_get_boolean(data_root, "dither");
	fog = json_object_get_boolean(data_root, "fog");

	config_menu = true;

	return true;
}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	
	//OPENGL initialization
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("[error] OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum glew_error = glewInit();
	if (glew_error != GL_NO_ERROR)
	{
		LOG("[error]Error initializing GLew! %s\n", glewGetErrorString(glew_error));
	}
	else LOG("Using GLew: %s", glewGetString(GLEW_VERSION));

	if(ret == true)
	{
		//Use Vsync
		if (vsync && SDL_GL_SetSwapInterval(1) < 0)
		{
			LOG("[error] Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}
		else if(!vsync)
		{
			SDL_GL_SetSwapInterval(0);
		}

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[error] Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[error] Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0, 0, 0, 1.f);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Enable OpenGL Options
		if (depth_test)
		{
			glEnable(GL_DEPTH_TEST);
		}

		if (cull_face)
		{
			glEnable(GL_CULL_FACE);
		}

		if (texture_2d)
		{
			glEnable(GL_TEXTURE_2D);
		}

		if (lighting)
		{
			glEnable(GL_LIGHTING);
		/*	GLfloat pos[4] = { 0.0f,0.0f,1.0f,0.0f };
			glLightfv(GL_LIGHT0, GL_POSITION, pos);

			GLfloat ambient[4] = { 0.0f,0.0f,1.0f,1.0f };
			glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);*/
		}

		if (color_material)
		{
			glEnable(GL_COLOR_MATERIAL);
			//glColorMaterial(GL_BACK, GL_AMBIENT);
		}

		if (dither)
		{
			glEnable(GL_DITHER);
		}

		if (fog)
		{
			glEnable(GL_FOG);
			const GLfloat color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			glFogfv(GL_FOG_COLOR, color);
			glFogf(GL_FOG_DENSITY, 0.0f);
		}

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[error] Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		float br = App->window->GetBrightness();
		GLfloat LightModelAmbient[] = { br, br, br, br };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
	
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
	
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->position.x, App->camera->position.y, App->camera->position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	
	// Rendering GUI
	ImGui::Render();

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::BlitConfigInfo()
{
	//Vsync check box
	if (ImGui::Checkbox("VSync", &vsync))
	{
		App->audio->PlayFxForInput(CHECKBOX_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turn ON/OFF VSync.");
	
	ImGui::Separator();

	if (ImGui::Button("Apply##renderer_apply", ImVec2(50, 20)))
	{
		//Save values
		//Load config json file
		const JSON_Value *config_data = App->fs->LoadJSONFile("config.json");
		assert(config_data != NULL);

		//Save the new variables
		json_object_set_boolean(App->fs->AccessObject(config_data, 1, name.c_str()), "vsync", vsync);
		
		//Save the file
		App->fs->SaveJSONFile(config_data, "config.json");
		json_value_free((JSON_Value*)config_data);

		if (vsync)
		{
			if (SDL_GL_SetSwapInterval(1) < 0)
			{
				LOG("[error] Unable to set VSync! SDL Error: %s\n", SDL_GetError());
			}
		}
		else SDL_GL_SetSwapInterval(0);

		//Play save fx
		App->audio->PlayFxForInput(FX_ID::APPLY_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Press Apply to save all the changes.");
}

// Functionality ================================
void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
