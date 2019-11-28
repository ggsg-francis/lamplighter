#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//for transform rotation
#include <glm\gtc\quaternion.hpp>

#include <vector>

#include "maths.hpp"
#include "graphics.hpp"

// Class representing a 3D transformation
class Transform3D
{
protected:
	m::Vector3 position = m::Vector3(0.f, 0.f, 0.f);
	m::Quaternion rotation = m::Quaternion(0.f, 0.f, 0.f, 1.f);
	m::Vector3 scale = m::Vector3(1.f, 1.f, 1.f);



public:

	glm::vec3 pos_glm = glm::vec3(0.f, 0.f, 0.f);
	glm::quat rot_glm = glm::quat(0.f, 0.f, 0.f, 1.f);
	glm::vec3 sca_glm = glm::vec3(1.f, 1.f, 1.f);

	// Move in global space
	void Translate(glm::vec3 offset)
	{
		pos_glm += offset;
		position = pos_glm;
	}

	// Move in global space
	void Translate(m::Vector3 offset)
	{
		pos_glm += glm::vec3(offset.x, offset.y, offset.z);
		position += offset;
	}

	// Move in local space
	void TranslateLocal(glm::vec3 offset)
	{
		position += m::RotateVector(offset, rotation);
		pos_glm.x = position.x;
		pos_glm.y = position.y;
		pos_glm.z = position.z;
	}

	// Move in local space
	void TranslateLocal(m::Vector3 offset)
	{
		position += m::RotateVector(offset, rotation);
		pos_glm.x = position.x;
		pos_glm.y = position.y;
		pos_glm.z = position.z;
	}

	// Modify relative rotation in global space
	void Rotate(float ANGLE, m::Vector3 AXIS);
	// Set absolute position in global space
	void SetPosition(m::Vector3 POSITION);
	// Set absolute rotation in global space
	void SetRotation(m::Quaternion ROTATION);
	// Set absolute rotation in 2D space
	void SetRotation(float ROTATION);
	// Set absolute scale in global space
	void SetScale(m::Vector3 SCALE);

	// Get absolute position in global space
	m::Vector3 GetPosition();
	// Get absolute rotation in global space
	m::Quaternion GetRotation();
	// Get absolute scale in global space
	m::Vector3 GetScale();

	// Get local X vector
	m::Vector3 GetRight();
	// Get local Y vector
	m::Vector3 GetUp();
	// Get local Z vector
	m::Vector3 GetForward();

	// Get GLM matrix
	glm::mat4 getModelMatrix();
	graphics::Matrix4x4 getMatrix();
};