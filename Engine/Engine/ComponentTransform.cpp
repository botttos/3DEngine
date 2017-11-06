#include "ComponentTransform.h"
#include "GameObject.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "imgui/ImGuizmo.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "Serializer.h"
// Constructors =================================
ComponentTransform::ComponentTransform() : Component(COMP_TRANSFORMATION)
{
}

ComponentTransform::ComponentTransform(const ComponentTransform & cpy) : Component(cpy), position(cpy.position), scale(cpy.scale), rotation_euler_angles(cpy.rotation_euler_angles), transform_matrix(cpy.transform_matrix)
{
}

// Destructors ==================================
ComponentTransform::~ComponentTransform()
{

}

// Game Loop ====================================
bool ComponentTransform::Update(float dt)
{
	if (has_been_modified)
	{
		UpdateTransform();
	}

	if (!parent->GetParent()->IsRoot())
	{
		ComponentTransform* tmp = ((ComponentTransform*)parent->GetParent()->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION));
		inherited_transform = tmp->inherited_transform * transform_matrix;
		inherited_position = tmp->position;
	}

	if (draw_axis && parent->IsSelectedObject())
	{
		DrawOrientationAxis();
	}

	/*static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	//if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(transform_matrix.ptr(), matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	ImGui::InputFloat3("Rt", matrixRotation, 3);
	ImGui::InputFloat3("Sc", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, transform_matrix.ptr());

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();
	math::float3 snap = { 10,10,10 };
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		//snap = config.mSnapTranslation;
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImGuizmo::ROTATE:
		//snap = config.mSnapRotation;
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImGuizmo::SCALE:
		//snap = config.mSnapScale;
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(550, 300, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(App->camera->editor_camera_frustrum.WorldMatrix().ptr(), App->camera->editor_camera_frustrum.ProjectionMatrix().ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, transform_matrix.ptr(), NULL, useSnap ? &snap.x : NULL);*/
	
	return true;
}

// Set Methods ==================================
void ComponentTransform::SetTransformation(aiMatrix4x4 trans)
{
	//Set the mathgeolib matrix from assim matrix
	float values[16] =
	{
		trans.a1, trans.a2, trans.a3, trans.a4,
		trans.b1, trans.b2, trans.b3, trans.b4,
		trans.c1, trans.c2, trans.c3, trans.c4,
		trans.d1, trans.d2, trans.d3, trans.d4
	};

	transform_matrix.Set(values);

	//Set the variables that will be shown in the UI
	transform_matrix.Decompose(position, rotation_quaternion, scale);
	rotation_euler_angles = (rotation_quaternion.ToEulerXYZ() * RADTODEG);

	//Set the inherited transform
	if (!(parent->GetParent()->IsRoot()))
	{
		ComponentTransform* tmp = ((ComponentTransform*)parent->GetParent()->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION));
		inherited_transform = tmp->inherited_transform * transform_matrix;
		inherited_position = tmp->position;
	}
	else
	{
		inherited_transform = transform_matrix;
	}
}

void ComponentTransform::SetTransformation(math::float4x4 trans)
{
	transform_matrix = trans;

	transform_matrix.Decompose(position, rotation_quaternion, scale);
	rotation_euler_angles = rotation_quaternion.ToEulerXYZ();


	//Set the inherited transform
	if (!(parent->GetParent()->IsRoot()))
	{
		ComponentTransform* tmp = ((ComponentTransform*)parent->GetParent()->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION));
		inherited_transform = tmp->inherited_transform * transform_matrix;
		inherited_position = tmp->position;
	}
	else
	{
		inherited_transform = trans;
	}
}

// Get Methods ==================================
math::float3 ComponentTransform::GetPosition() const
{
	return position;
}

math::float3 ComponentTransform::GetRotationEuler() const
{
	return rotation_euler_angles;
}

math::Quat ComponentTransform::GetRotationQuat() const
{
	return rotation_quaternion;
}

math::float3 ComponentTransform::GetScale() const
{
	return scale;
}

math::float4x4 ComponentTransform::GetTransform() const
{
	return transform_matrix;
}

math::float4x4 ComponentTransform::GetInheritedTransform() const
{
	return inherited_transform;
}

const float* ComponentTransform::GetTransformMatrixRows() const
{
	return transform_matrix.ptr();
}

const float* ComponentTransform::GetTransformMatrixColumns() const
{
	return transform_matrix.Transposed().ptr();
}


// Functionality ================================
void ComponentTransform::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::Checkbox("##transform_comp", &actived);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Transform");
	ImGui::SameLine();
	ImGui::Checkbox("Draw Axis##transform", &draw_axis);

	//Transform Position
	ImGui::Text("Position ");
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	if (ImGui::DragFloat("X##position", &position.x, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Y##position", &position.y, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Z##position", &position.z, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;

	//Transform rotation
	bool has_rotate = false;
	ImGui::Text("Rotation ");
	ImGui::SameLine();
	if (ImGui::DragFloat("X##rotation", &rotation_euler_angles.x, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Y##rotation", &rotation_euler_angles.y, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Z##rotation", &rotation_euler_angles.z, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;

	//Transform scale
	ImGui::Text("Scale    ");
	ImGui::SameLine();
	if (ImGui::DragFloat("X##scale", &scale.x, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Y##scale", &scale.y, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Z##scale", &scale.z, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::PopItemWidth();

	ImGui::Text("Normalized Scale");
	float norm = (scale.x + scale.y + scale.z) / 3.0f;
	if (ImGui::DragFloat("Normalized Scale", &norm, 0.2f, 0.1f))
	{
		if (norm < 0.01f)norm = 0.01f;
		scale.SetFromScalar(norm);
		has_been_modified = true;
	}
}

void ComponentTransform::UpdateTransform()
{
	rotation_quaternion = math::Quat::FromEulerXYZ(rotation_euler_angles.x * DEGTORAD, rotation_euler_angles.y  * DEGTORAD, rotation_euler_angles.z  * DEGTORAD);

	transform_matrix = math::float4x4::FromQuat(rotation_quaternion);
	transform_matrix = math::float4x4::Scale(scale, math::float3(0, 0, 0)) * transform_matrix;
	transform_matrix.SetTranslatePart(position.x, position.y, position.z);

	//If its parent is scene update inherited matrix

	if (parent->GetParent()->IsRoot())
	{
		inherited_transform = transform_matrix;
		inherited_position = position;
		parent->GetBoundingBox()->Scale(parent->GetBoundingBox()->CenterPoint(),scale);
	}


	has_been_modified = false;
}

void ComponentTransform::SetMatrixToDraw()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(inherited_transform.Transposed().ptr());
	

}

void ComponentTransform::QuitMatrixToDraw()
{
	glPopMatrix();
}

void ComponentTransform::DrawOrientationAxis() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	//AXIS X
	math::float3 axis = inherited_transform.Col3(0).Normalized();
	math::float3 pos;

	if (!(parent->GetParent()->IsRoot()))
	{
		pos = inherited_position + position;
	}
	else
	{
		pos = position;
	}
	
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(axis.x + pos.x, axis.y + pos.y, axis.z + pos.z);

	//AXIS Y
	axis = inherited_transform.Col3(1).Normalized();
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(axis.x + pos.x, axis.y + pos.y, axis.z + pos.z);

	//AXIS Z
	axis = inherited_transform.Col3(2).Normalized();
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(axis.x + pos.x, axis.y + pos.y, axis.z + pos.z);

	glEnd();

	glLineWidth(1.0f);
}

bool ComponentTransform::Save(Serializer & array_root) const
{
	bool ret = false;

	//Serializer where all the data of the component is built
	Serializer comp_data;

	//Insert Component Type
	ret = comp_data.InsertString("type", ComponentTypeToStr(type));
	//Insert component id
	ret = comp_data.InsertInt("id", id);
	//Insert actived
	ret = comp_data.InsertBool("actived", actived);

	//Insert transform matrix
	Serializer trans_matrix_array = comp_data.InsertArray("transform_matrix");
	for (uint k = 0; k < 16; k++)trans_matrix_array.InsertArrayFloat(transform_matrix.ptr()[k]);
	//Insert position
	Serializer position_array = comp_data.InsertArray("position");
	for (uint k = 0; k < 3; k++)position_array.InsertArrayFloat(position.ptr()[k]);
	//Insert inherited transform matrix
	Serializer inherited_trans_array = comp_data.InsertArray("inherited_transform");
	for (uint k = 0; k < 16; k++)inherited_trans_array.InsertArrayFloat(inherited_transform.ptr()[k]);
	//Insert inherited position
	Serializer inherited_position_array = comp_data.InsertArray("inherited_position");
	for (uint k = 0; k < 3; k++)inherited_position_array.InsertArrayFloat(inherited_position.ptr()[k]);
	//Insert scale
	Serializer scale_array = comp_data.InsertArray("scale");
	for (uint k = 0; k < 3; k++)scale_array.InsertArrayFloat(scale.ptr()[k]);
	//Insert rotation quaternion
	Serializer rot_quaternion_array = comp_data.InsertArray("rotation_quaternion");
	for (uint k = 0; k < 4; k++)rot_quaternion_array.InsertArrayFloat(rotation_quaternion.ptr()[k]);

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool ComponentTransform::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
{
	bool ret = true;

	//Get component id
	id = data.GetInt("id");
	//Get actived
	actived = data.GetBool("actived");

	//Get transform matrix
	Serializer trans_matrix_array = data.GetArray("transform_matrix");
	for (uint k = 0; k < 16; k++)transform_matrix.ptr()[k] = trans_matrix_array.GetArrayFloat(k);
	//Get position
	Serializer position_array = data.GetArray("position");
	for (uint k = 0; k < 3; k++)position.ptr()[k] = position_array.GetArrayFloat(k);
	//Get inherited transform matrix
	Serializer inherited_trans_array = data.GetArray("inherited_transform");
	for (uint k = 0; k < 16; k++)inherited_transform.ptr()[k] = inherited_trans_array.GetArrayFloat(k);
	//Get inherited position
	Serializer inherited_position_array = data.GetArray("inherited_position");
	for (uint k = 0; k < 3; k++)inherited_position.ptr()[k] = inherited_position_array.GetArrayFloat(k);
	//Get scale
	Serializer scale_array = data.GetArray("scale");
	for (uint k = 0; k < 3; k++)scale.ptr()[k] = scale_array.GetArrayFloat(k);
	//Get rotation quaternion
	Serializer rot_quaternion_array = data.GetArray("rotation_quaternion");
	for (uint k = 0; k < 4; k++)rotation_quaternion.ptr()[k] = rot_quaternion_array.GetArrayFloat(k);

	return ret;
}
