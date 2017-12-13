#ifndef _COMPONENT_PARTICLES_H_
#define _COMPONENT_PARTICLES_H_

#include "Component.h"
#include "Globals.h"
#include <stdio.h>
#include <math.h>
#include "ComponentMaterial.h"

struct Particle
{
	float x_pos;
	float y_pos;
	float z_pos;
	float x_mov;
	float y_mov;
	float z_mov;
	float red;
	float green;
	float blue;
	float direction;
	float acceleration;
	float deceleration;
	float scale;
};

class ComponentParticle : public Component
{
public:
	ComponentParticle();
	~ComponentParticle();

	bool Start();
	bool Update(float dt);
	bool Draw();
	
	//Utility
	void ResetParticles(Particle& p);
	void ApplyParticleChanges();
	void BlitComponentInspector();
	
private:
	const int particle_count = 500;
	uint texture_id;
	Particle particles[500];
	Particle modified_particle;
};

#endif // !_COMPONENT_PARTICLES_H_