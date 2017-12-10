#ifndef _COMPONENT_PARTICLES_H_
#define _COMPONENT_PARTICLES_H_

#include "Engine\Component.h"
#include <gl\GL.h>
#include "Engine\glut\glut.h"
#include <windows.h>
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

private:
	int particle_count = 500;

	GLfloat texture[10];

	std::vector<Particle> particles;
};

#endif // !_COMPONENT_PARTICLES_H_