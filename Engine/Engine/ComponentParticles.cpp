#include "ComponentParticles.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "GameObject.h"
#include "MathGeoLib/Math/MathAll.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ImporterManager.h"
#include "ResourcesManager.h"
#include "Serializer.h"
#include "ModuleScene.h"


ComponentParticle::ComponentParticle()
{
}

ComponentParticle::~ComponentParticle()
{
}

bool ComponentParticle::Start()
{
	//Pick texture
	//id_texture =
	emission_ot.Start();
	
	for (int i = 0; i < p_count; i++)
	{
		//Position
		ComponentTransform* comp_transform = (ComponentTransform*)parent->FindComponent(COMP_TRANSFORMATION);
		math::float3 position = comp_transform->GetPosition();

		particles[i].pos.x = position.x;
		particles[i].pos.y = position.y;
		particles[i].pos.z = position.z;

		//Movement with random
		particles[i].mov.x = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004);
		particles[i].mov.z = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004);

		//Set RGB colors
		particles[i].color.x = 1;
		particles[i].color.y = 1;
		particles[i].color.z = 1;

		//Scale
		particles[i].scale = 0.25;

		//Initial rotation
		particles[i].direction = 0;

		//Acceleration with random
		particles[i].acceleration = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004) -(((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004);

		//Deceleration
		particles[i].deceleration = 0.0025;

		//Life time
		particles[i].life_time.Start();
	}
	modified_particle = particles[0];
	return true;
}

bool ComponentParticle::Update(float dt)
{
	if (App->scene->scene_paused == false)
	{
		//Restart timers if particle was paused
		if (paused == true)
		{
			paused = false;
			emission_ot.Start();
			for (int i = 0; i < particles_on_scene; i++)
			{
				particles[i].life_time.Start();
			}
		}
		
		// used 1- to invert the Emission over time parameter on UI
		if (emission_ot.ReadSec() > (1 - p_emission_ot))
		{
			particles_on_scene++;
			emission_ot.Start();
		}

		//Orientating particles
		if (App->camera != nullptr)
		{
			ComponentTransform* parent_pos = (ComponentTransform*)parent->FindComponent(COMP_TRANSFORMATION);
			if (parent_pos != nullptr)
			{
				math::float3 look = App->camera->GetPosition() - parent_pos->GetPosition();
				rotation = math::Quat::LookAt(math::float3(0.0f, 0.0f, 1.0f), look.Normalized(), math::float3(0.0f, 1.0f, 0.0f), math::float3(0.0f, 1.0f, 0.0f));
			}
		}

		//Update particles
		for (int i = 0; i < particles_on_scene; i++)
		{
			//Set the color of the particle
			glColor3f(particles[i].color.x, particles[i].color.y, particles[i].color.z);

			//Move particle
			particles[i].pos.y += ((particles[i].acceleration + p_acceleration) - (particles[i].deceleration + p_deceleration));
			particles[i].deceleration -= (0.000025 + p_deceleration);

			particles[i].pos.x += particles[i].mov.x + p_mov_x;
			particles[i].pos.z += particles[i].mov.z + p_mov_z;

			if (particles[i].life_time.ReadSec() > p_lifetime)
			{
				particles_on_scene--;
				ResetParticle(particles[i]);
			}
		}
	}
	else if (paused == false)
	{
		//Pause timers
		paused = true;
		emission_ot.Stop();
		for (int i = 0; i < particles_on_scene; i++)
		{
			particles[i].life_time.Stop();
		}
	}

	//Draw particles
	Draw();
	return true;
}

bool ComponentParticle::Draw()
{
	for (int i = 0; i < particles_on_scene; i++)
	{
		glPushMatrix();

		//Translate x y z axis coords
		glTranslatef(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z);

		//Rotate particle
		math::float3 vec_rotation = rotation.ToEulerXYZ();
		glRotatef(RADTODEG*vec_rotation.x, 1, 0, 0);
		glRotatef(RADTODEG*vec_rotation.y, 0, 1, 0);
		glRotatef(RADTODEG*vec_rotation.z - 180, 0, 0, 1);

		//Scale particle
		glScalef(particles[i].scale, particles[i].scale, particles[i].scale);

		//Disable depth texting and enable blend
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, App->textures->garbage_icon);

		//Drawing shape
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3f(-1, -1, 0);
		glTexCoord2d(1, 0);
		glVertex3f(1, -1, 0);
		glTexCoord2d(1, 1);
		glVertex3f(1, 1, 0);
		glTexCoord2d(0, 1);
		glVertex3f(-1, 1, 0);
		glEnd();

		//Enable depth testing again and end particle changes
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
	}
	return false;
}

bool ComponentParticle::Save(Serializer & array_root) const
{
	bool ret = false;

	//Serializer where all the data of the component is built
	Serializer comp_data;

	//Insert Component Type
	ret = comp_data.InsertString("type", ComponentTypeToStr(COMP_PARTICLE));
	//Insert component id
	ret = comp_data.InsertInt("id", id);
	//Insert actived
	ret = comp_data.InsertBool("actived", actived);

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool ComponentParticle::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
{
	bool ret = true;

	//Get component id
	id = data.GetInt("id");
	//Get actived
	actived = data.GetBool("actived");

	return ret;
}

void ComponentParticle::ResetParticle(Particle& p)
{
	//Position
	ComponentTransform* comp_transform = (ComponentTransform*)parent->FindComponent(COMP_TRANSFORMATION);
	math::float3 position = comp_transform->GetPosition();
	
	p.pos.x = position.x + modified_particle.pos.x;
	p.pos.y = position.y + modified_particle.pos.y;
	p.pos.z = position.z + modified_particle.pos.z;

	//Acceleration with random
	p.acceleration = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005);
	//Deceleration
	p.deceleration = 0.0025;

	//Scale 
	p.scale = modified_particle.scale;

	//Life Time
	p.life_time.Start();
}

void ComponentParticle::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::Checkbox("##particle_component", &actived);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Particle Emmisor");
	ImGui::SameLine();

	//Particle Position
	ImGui::Text("Position ");
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	char name[200];
	sprintf(name, "X## %i", id);
	if (ImGui::DragFloat(name, &modified_particle.pos.x, 0.5f, 0.0f, 0.0f, "%.1f"));
	ImGui::SameLine();
	sprintf(name, "Y## %i", id);
	if (ImGui::DragFloat(name, &modified_particle.pos.y, 0.5f, 0.0f, 0.0f, "%.1f"));
	ImGui::SameLine();
	sprintf(name, "Z## %i", id);
	if (ImGui::DragFloat(name, &modified_particle.pos.z, 0.5f, 0.0f, 0.0f, "%.1f"));

	//Particle norm Scale
	ImGui::Text("Scale");
	sprintf(name, "scale## %i", id);
	if (ImGui::DragFloat(name, &modified_particle.scale, 0.1f, 0.1f));

	//Life time
	ImGui::Text("Life time");
	sprintf(name, "lifetime## %i", id);
	if (ImGui::SliderFloat(name, &p_lifetime, 0.1f, 1.0f));

	//Acceleration
	ImGui::Text("Acceleration");
	sprintf(name, "acceleration## %i", id);
	if (ImGui::SliderFloat(name, &p_acceleration, -0.5f, 0.5f));

	//Deceleration
	ImGui::Text("Deceleration");
	sprintf(name, "deceleration## %i", id);
	if (ImGui::SliderFloat(name, &p_deceleration, -0.2f, 0.2f));
	
	//X and Z directions
	ImGui::Text("X direction");
	sprintf(name, "xmov## %i", id);
	if (ImGui::SliderFloat(name, &p_mov_x, -0.5f, 0.5f));

	ImGui::Text("Z direction");
	sprintf(name, "zmov## %i", id);
	if (ImGui::SliderFloat(name, &p_mov_z, -0.5f, 0.5f));

	//Emission over time
	ImGui::Text("Emission over time");
	sprintf(name, "emission over time## %i", id);
	if (ImGui::SliderFloat(name, &p_emission_ot, 0.0f, 10.0f));

	// Sprites
	std::vector<ResourceMaterial*> all_materials = App->res_manager->FindTextures();
	bool opened = ImGui::TreeNodeEx("Sprites", ImGuiTreeNodeFlags_OpenOnDoubleClick);
	if (opened)
	{
		uint size = all_materials.size();
		for (uint k = 0; k < size; k++)
		{
			
		}

		ImGui::TreePop();
	}
	
	
	/*
	if (ImGui::MenuItem("Sprites"))
	{
		std::vector<ResourceMaterial*> all_materials = App->res_manager->FindTextures();
		
		uint size = all_materials.size();
		else
		{
			for (vector<ResourceMaterial*>::const_iterator res = all_materials.begin(); res != all_materials.end(); res++)
			{
				
			}
		}
	}
	*\
	/*if (ImGui::Button("Save Changes"))
		ApplyParticleChanges();*/
}