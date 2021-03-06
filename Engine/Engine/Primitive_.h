#ifndef _BASIC_PRIMITIVE_H_
#define _BASIC_PRIMITIVE_H_

#include "Globals.h"
#include "Color.h"
#include "MathGeoLib/MathGeoLib.h"
#include "ComponentMesh.h"

enum PRIMITIVE_TYPE
{
	UNDEF_PRIMITIVE = 0,
	PRIMITIVE_CUBE,
	PRIMITIVE_SPHERE,
	PRIMITIVE_SPHERE_HI,
	PRIMITIVE_CYLINDER,
	PRIMITIVE_CYLINDER_HI,
	PRIMITIVE_CAPSULE,
	PRIMITIVE_FRUSTUM,
	PRIMITIVE_GRID
};

struct SimpleMesh
{
	virtual ~SimpleMesh()
	{
		delete []indices;
		delete []vertices;
	}

	uint	id_vertices = 0; // id in VRAM
	uint	num_indices = 0;
	uint*	indices = nullptr;

	uint	id_indices = 0; // id in VRAM
	uint	num_vertices = 0;
	float*	vertices = nullptr;

};

class Primitive_ : public SimpleMesh
{

	friend class GeometryManager;

public:

	Primitive_(PRIMITIVE_TYPE _type);
	Primitive_(const Primitive_& _cpy);
	~Primitive_();

public:

	virtual void	Initialize();
	virtual void	Draw();
	void			DrawAxis()const;

protected:

	std::vector<Vertex>	n_vertices;
	std::vector<uint>	n_indices;

	Color			color = { 255,255,255,255 };
	bool			axis = false;
	PRIMITIVE_TYPE	type = UNDEF_PRIMITIVE;
	uint			divisions = 0;

public:

	//Set Methods -----------
	void SetColor(const Color& n_color);
	void SetAxis(bool n_axis);
	void SetType(PRIMITIVE_TYPE n_type);
	void SetDivisions(uint def);

	//Get Methods -----------
	Color				GetColor()const;
	bool				GetAxis()const;
	PRIMITIVE_TYPE		GetType()const;
	std::vector<uint>	GetIndices()const;
	std::vector<Vertex> GetVertex()const;

};

bool VertexToIndex(math::float3* all_vertex, uint vertex_num, std::vector<uint>* index, std::vector<math::float3>* vertex);

const char*		PrimitiveTypeToStr(PRIMITIVE_TYPE type);
PRIMITIVE_TYPE	StrToPrimitiveType(const char* str);

#endif // !_BASIC_PRIMITIVE_H_
