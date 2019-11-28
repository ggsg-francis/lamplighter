#include "Transform.h"

void Transform3D::Rotate(float angle, m::Vector3 axis)
{
	SetRotation(m::Rotate(rotation, angle, axis));
}

void Transform3D::SetPosition(m::Vector3 v)
{
	position = v;
	pos_glm.x = v.x;
	pos_glm.y = v.y;
	pos_glm.z = v.z;
}

void Transform3D::SetRotation(m::Quaternion q)
{
	rotation = q;
	rot_glm.x = q.x;
	rot_glm.y = q.y;
	rot_glm.z = q.z;
	rot_glm.w = q.w;
}

void Transform3D::SetRotation(float rot)
{
	m::Quaternion q = m::Rotate(m::Quaternion(0,0,0,1), rot, m::Vector3(0,1,0));
	rotation = q;
	rot_glm.x = q.x;
	rot_glm.y = q.y;
	rot_glm.z = q.z;
	rot_glm.w = q.w;
}

void Transform3D::SetScale(m::Vector3 v)
{
	scale = v;
	sca_glm.x = v.x;
	sca_glm.y = v.y;
	sca_glm.z = v.z;
}

m::Vector3 Transform3D::GetPosition()
{
	return position;
}

m::Quaternion Transform3D::GetRotation()
{
	return rotation;
}

m::Vector3 Transform3D::GetScale()
{
	return scale;
}

m::Vector3 Transform3D::GetRight()
{
	return m::RotateVector(m::Vector3(1, 0, 0), rotation);
}

m::Vector3 Transform3D::GetUp()
{
	return m::RotateVector(m::Vector3(0, 1, 0), rotation);
}

m::Vector3 Transform3D::GetForward()
{
	return m::RotateVector(m::Vector3(0, 0, 1), rotation);
}

glm::mat4 Transform3D::getModelMatrix()
{
	glm::mat4 model = glm::mat4(1.0f); // Create identity matrix
	model = glm::scale(model, glm::vec3(1.f, 1.f, -1.f)); // See if we can remove this extra transformation!
	model = glm::translate(model, pos_glm);
	model *= glm::mat4_cast(rot_glm);
	model = glm::scale(model, sca_glm);
	return model;
}

graphics::Matrix4x4 Transform3D::getMatrix()
{
	glm::mat4 model = glm::mat4(1.0f); // Create identity matrix
	model = glm::scale(model, glm::vec3(1.f, 1.f, -1.f)); // See if we can remove this extra transformation!
	model = glm::translate(model, pos_glm);
	model *= glm::mat4_cast(rot_glm);
	model = glm::scale(model, sca_glm);
	graphics::Matrix4x4 model2;
	model2[0][0] = model[0][0];
	model2[0][1] = model[0][1];
	model2[0][2] = model[0][2];
	model2[0][3] = model[0][3];

	model2[1][0] = model[1][0];
	model2[1][1] = model[1][1];
	model2[1][2] = model[1][2];
	model2[1][3] = model[1][3];

	model2[2][0] = model[2][0];
	model2[2][1] = model[2][1];
	model2[2][2] = model[2][2];
	model2[2][3] = model[2][3];

	model2[3][0] = model[3][0];
	model2[3][1] = model[3][1];
	model2[3][2] = model[3][2];
	model2[3][3] = model[3][3];
	return model2;
}