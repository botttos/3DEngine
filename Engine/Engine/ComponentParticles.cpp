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
	for (int i = 0; i < particle_count; i++)
	{
		//Position
		ComponentTransform* comp_transform = (ComponentTransform*)parent->FindComponent(COMP_TRANSFORMATION);
		math::float3 position = comp_transform->GetPosition();

		particles[i].x_pos = position.x;
		particles[i].y_pos = position.y;
		particles[i].z_pos = position.z;

		//Movement with random
		particles[i].x_mov = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005);
		particles[i].z_mov = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005);

		//Set RGB colors
		particles[i].red = 1;
		particles[i].green = 1;
		particles[i].blue = 1;

		//Scale
		particles[i].scale = 0.25;

		//Initial rotation
		particles[i].direction = 0;

		//Acceleration with random
		particles[i].acceleration = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005) -(((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005);

		//Deceleration
		particles[i].deceleration = 0.0025;

	}
	return true;
}

bool ComponentParticle::Update(float dt)
{
	Draw();

	for (int i = 0; i < particle_count; i++)
	{
		//Set the color of the particle
		glColor3f(particles[i].red, particles[i].green, particles[i].blue);

		//Move particle
		particles[i].y_pos += (particles[i].acceleration - particles[i].deceleration);
		particles[i].deceleration += 0.0025;

		particles[i].x_pos += particles[i].x_mov;
		particles[i].z_pos += particles[i].z_mov;

		//Rotate particle
		particles[i].direction += (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005);

		if (particles[i].y_pos < -5 )
			ResetParticles(particles[i]);
	}
	return true;
}

bool ComponentParticle::Draw()
{
	for (int i = 0; i < particle_count; i++)
	{
		glPushMatrix();

		//Translate x y z axis coords
		glTranslatef(particles[i].x_pos, particles[i].y_pos, particles[i].z_pos);

		//Rotate particle
		glRotatef(particles[i].direction - 90, 0, 0, 1);

		//Scale particle
		glScalef(particles[i].scale, particles[i].scale, particles[i].scale);

		//Disable depth texting and enable blend
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		glBlendFunc(GL_DST_COLOR, GL_ZERO);
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
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();

	}
	return false;
}

void ComponentParticle::ResetParticles(Particle& p)
{
	//Position
	ComponentTransform* comp_transform = (ComponentTransform*)parent->FindComponent(COMP_TRANSFORMATION);
	math::float3 position = comp_transform->GetPosition();
	
	p.x_pos = position.x;
	p.y_pos = position.y;
	p.z_pos = position.z;

	//Set RGB colors
	p.red = 1;
	p.green = 1;
	p.blue = 1;

	//Initial rotation
	p.direction = 0;

	//Acceleration with random
	p.acceleration = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005);
	//Deceleration
	p.deceleration = 0.0025;
}

void ComponentParticle::BlitComponentInspector()
{

}
