#ifndef _COMPONENT_PARTICLES_H_
#define _COMPONENT_PARTICLES_H_

#include "Component.h"
#include "Globals.h"
#include <stdio.h>
#include <math.h>
#include <vector>

struct Particle
{
	double x_pos;
	double y_pos;
	double z_pos;
	double x_mov;
	double y_mov;
	double z_mov;
	double red;
	double green;
	double blue;
	double direction;
	double acceleration;
	double deceleration;
	double scale;
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
	void ResetParticles(Particle p);

private:
	const int particle_count = 500;
	uint texture_id;
	Particle particles[500];
};

#endif // !_COMPONENT_PARTICLES_H_