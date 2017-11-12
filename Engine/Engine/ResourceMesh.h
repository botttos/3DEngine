#ifndef _RESOURCE_MESH_H_
#define _RESOURCE_MESH_H_

#include "Resource.h"
#include "ComponentMesh.h"
#include <vector>

class ResourceMesh : public Resource
{
public:

	ResourceMesh();
	~ResourceMesh();

private:

	std::vector<Vertex>			vertices;
	std::vector<uint>			indices;

	uint						num_tris = 0;
	uint						num_vertex = 0;

	uint VertexArrayObject = 0, VertexBufferObject = 0, ElementBufferObject = 0;

	uint face_normalsID = 0;
	uint vertex_normalsID = 0;
	uint text_coordsID = 0;

public:

	//Get Methods -----------
	std::vector<math::float3>	GetVertexPositions()const;
	uint						GetIndexSize()const;
	int							GetIndexAt(int position) const;
	math::float3				GetVertexPosAt(int position) const;
	uint						GetNumTris()const;
	uint						GetNumVertex()const;
	uint						GetVertexArrayObject()const;
	uint						GetVertexBufferObject()const;
	uint						GetElementBufferObject()const;
	uint						GetFaceNormalsID()const;
	uint						GetVertexNormalsID()const;
	uint						GetTextureCoordsID()const;

	//Set Methods -----------
	void	SetVertices(const std::vector<Vertex>& v);
	void	SetIndices(const std::vector<uint>& i);

	//Functionality ---------
	void	SetupMesh();
	void	DeleteBuffers();

	//Save/Load Methods -----
	bool	Save(Serializer& array_root)const;
	bool	Load(Serializer& data);
};

#endif // !_RESOURCE_MESH_H_