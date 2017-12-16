#include "ComponentParticles.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "GameObject.h"
#include "MathGeoLib/Math/MathAll.h"
#include "ComponentTransform.h"

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
		particles[i].x_mov = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004);
		particles[i].z_mov = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004);

		//Set RGB colors
		particles[i].red = 1;
		particles[i].green = 1;
		particles[i].blue = 1;

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
	// (1 - p_emission_ot) is used to invert the Emission over time parameter on UI
	if (emission_ot.ReadSec() > (1 - p_emission_ot)) 
	{
		particles_on_scene++;
		emission_ot.Start();
	}

	//Draw particles
	Draw();
	
	//Update particles
	for (int i = 0; i < particles_on_scene; i++)
	{
		//Set the color of the particle
		glColor3f(particles[i].red, particles[i].green, particles[i].blue);

		//Move particle
		particles[i].pos.y += ((particles[i].acceleration + p_acceleration) - (particles[i].deceleration + p_deceleration));
		particles[i].deceleration -= (0.000025 + p_deceleration);

		particles[i].pos.x += particles[i].x_mov;
		particles[i].pos.z += particles[i].z_mov;

		//Rotate particle
		particles[i].direction += (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.004);

		if (particles[i].life_time.ReadSec() > p_lifetime)
		{
			particles_on_scene--;
			ResetParticle(particles[i]);
		}	
	}
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
		glRotatef(particles[i].direction - 90, 0, 0, 1);

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
	
	//Number of particles
	ImGui::Text("Number of particles");
	sprintf(name, "number of particles## %i", id);
	if (ImGui::DragInt(name, &p_count, 1, 0.001f));
	if(p_count > 500)
		p_count = 500;

	//Life time
	ImGui::Text("Life time");
	sprintf(name, "lifetime## %i", id);
	if (ImGui::DragFloat(name, &p_lifetime, 0.1f, 0.001f));

	//Deceleration
	ImGui::Text("Deceleration");
	sprintf(name, "deceleration## %i", id);
	if (ImGui::DragFloat(name, &p_deceleration, 0.0001f, 0.001f));

	//Acceleration
	ImGui::Text("Acceleration");
	sprintf(name, "acceleration## %i", id);
	if (ImGui::DragFloat(name, &p_acceleration, 0.0001f, 0.001f));

	//Emission over time
	ImGui::Text("Emission over time");
	sprintf(name, "emission over time## %i", id);
	if (ImGui::DragFloat(name, &p_emission_ot, 0.01f, 0.001f));

	/*if (ImGui::Button("Save Changes"))
		ApplyParticleChanges();*/
}
