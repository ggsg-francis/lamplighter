#include "Transform.h"

void Transform3D::Rotate(float angle, fw::Vector3 axis)
{
	SetRotation(fw::Rotate(rotation, angle, axis));
}

void Transform3D::SetPosition(fw::Vector3 v)
{
	position = v;
	pos_glm.x = v.x;
	pos_glm.y = v.y;
	pos_glm.z = v.z;
}

void Transform3D::SetRotation(fw::Quaternion q)
{
	rotation = q;
	rot_glm.x = q.x;
	rot_glm.y = q.y;
	rot_glm.z = q.z;
	rot_glm.w = q.w;
}

void Transform3D::SetRotation(float rot)
{
	fw::Quaternion q = fw::Rotate(fw::Quaternion(0,0,0,1), rot, fw::Vector3(0,1,0));
	rotation = q;
	rot_glm.x = q.x;
	rot_glm.y = q.y;
	rot_glm.z = q.z;
	rot_glm.w = q.w;
}

void Transform3D::SetScale(fw::Vector3 v)
{
	scale = v;
	sca_glm.x = v.x;
	sca_glm.y = v.y;
	sca_glm.z = v.z;
}

fw::Vector3 Transform3D::GetPosition()
{
	return position;
}

fw::Quaternion Transform3D::GetRotation()
{
	return rotation;
}

fw::Vector3 Transform3D::GetScale()
{
	return scale;
}

fw::Vector3 Transform3D::GetRight()
{
	return fw::RotateVector(fw::Vector3(1, 0, 0), rotation);
}

fw::Vector3 Transform3D::GetUp()
{
	return fw::RotateVector(fw::Vector3(0, 1, 0), rotation);
}

fw::Vector3 Transform3D::GetForward()
{
	return fw::RotateVector(fw::Vector3(0, 0, 1), rotation);
}

glm::mat4 Transform3D::getModelMatrix()
{
	glm::mat4 model = glm::mat4(1.0f); // Create identity matrix
	model = glm::scale(model, glm::vec3(1.f, 1.f, -1.f)); // See if we can remove this extra transformation!
	model = glm::translate(model, pos_glm);
	model *= glm::mat4_cast(rot_glm);
	model = glm::scale(model, sca_glm);
	model = glm::scale(model, glm::vec3(-1.f, 1.f, 1.f)); // See if we can remove this extra transformation!
	return model;
}
