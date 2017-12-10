#include "ComponentParticles.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"


ComponentParticle::ComponentParticle()
{
}

ComponentParticle::~ComponentParticle()
{
}

bool ComponentParticle::Start()
{
	for (int i = 0; i < particle_count; i++)
	{
		//Position
		particles[i].x_pos = 0;
		particles[i].y_pos = -5;
		particles[i].z_pos = -5;

		//Movement with random
		particles[i].x_mov = (rand() % 11);
		particles[i].z_mov = (rand() % 11);

		//Set RGB colors
		particles[i].red = 1;
		particles[i].green = 1;
		particles[i].blue = 1;

		//Scale
		particles[i].scale = 0.25;

		//Initial rotation
		particles[i].direction = 0;

		//Acceleration with random
		particles[i].acceleration = (rand() % 11);

		//Deceleration
		particles[i].deceleration = 0.0025;

	}
	return true;
}

bool ComponentParticle::Update(float dt)
{
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
		particles[i].direction += (rand() % 11);
	}
	return true;
}
