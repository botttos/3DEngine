#ifndef _COMPONENT_PARTICLES_H_
#define _COMPONENT_PARTICLES_H_

#include "Component.h"
#include "Globals.h"
#include <stdio.h>
#include <math.h>
#include "ComponentMaterial.h"
#include "Timer.h"
#include "MathGeoLib\Math\float3.h"

struct Particle
{
	math::float3 pos;
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

	Timer life_time;
};

class ComponentParticle : public Component
{
public:
	ComponentParticle();
	~ComponentParticle();

	bool Start();
	bool Update(float dt);
	bool Draw();
	
	//Utility ------------------------------
	void ResetParticle(Particle& p);
	void BlitComponentInspector();
	
private:
	uint texture_id;
	Particle particles[500];
	Particle modified_particle;

	//Modificable parameters ----------------
	float p_lifetime = 1;
	int p_count = 500; //Number of particles
	float p_deceleration = 0;
	float p_acceleration = 0;
	//Emission over time
	Timer emission_ot;
	float p_emission_ot = 0.8;
	int particles_on_scene = 0;

	/*Modificable parameters:
	speed
	lifetime
	gravity
	max particles
	emission over time
	velocity over lifetime (x, y, z axis)
	*/
};

#endif // !_COMPONENT_PARTICLES_H_