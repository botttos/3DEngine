#ifndef _COMPONENT_PARTICLES_H_
#define _COMPONENT_PARTICLES_H_

#include "Component.h"
#include "Globals.h"
#include <stdio.h>
#include <math.h>
#include "ComponentMaterial.h"
#include "Timer.h"
#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Math\Quat.h"

class ResourceMaterial;

struct Particle
{
	math::float3	pos;
	math::float3	mov;
	math::float3	color;
	float			direction;
	float			acceleration;
	float			deceleration;
	float			scale;
	Timer			life_time;
};

class ComponentParticle : public Component
{
public:
	ComponentParticle();
	~ComponentParticle();

	bool			Start();
	bool			Update(float dt);
	bool			Draw();
	virtual bool	Save(Serializer& array_root)const;
	virtual bool	Load(Serializer& data, std::vector<std::pair<Component*, uint>>& links);

private:
	//Utility ------------------------------
	void			ResetParticle(Particle& p);
	void			BlitComponentInspector();
	void			OrientToCamera();
	
private:
	math::Quat		rotation = math::Quat::identity;
	uint			texture_id;
	Particle		particles[500];
	Particle		modified_particle;

	//Modificable parameters ----------------
	float			p_lifetime = 1;
	int				p_count = 500; //Number of particles
	float			p_deceleration = 0;
	float			p_acceleration = 0;
	float			p_mov_x = 0;
	float			p_mov_z = 0;
	//Emission over time
	Timer			emission_ot;
	float			p_emission_ot = 0.8;
	int				particles_on_scene = 0;
	//On pause
	bool			paused = false;

	//Firework
	Timer			explode_timer;
	float			time_to_explode = 1;
};

#endif // !_COMPONENT_PARTICLES_H_