#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//for transform rotation
#include <glm\gtc\quaternion.hpp>

#include <vector>

#include "maths.hpp"

// Class representing a 3D transformation
class Transform3D
{
protected:
	fw::Vector3 position = fw::Vector3(0.f, 0.f, 0.f);
	fw::Quaternion rotation = fw::Quaternion(0.f, 0.f, 0.f, 1.f);
	fw::Vector3 scale = fw::Vector3(1.f, 1.f, 1.f);

	glm::vec3 pos_glm = glm::vec3(0.f, 0.f, 0.f);
	glm::quat rot_glm = glm::quat(1.f, 0.f, 0.f, 0.f);
	glm::vec3 sca_glm = glm::vec3(1.f, 1.f, 1.f);

public:

	// Move in global space
	void Translate(glm::vec3 offset)
	{
		pos_glm += offset;
		position = pos_glm;
	}

	// Move in global space
	void Translate(fw::Vector3 offset)
	{
		pos_glm += glm::vec3(offset.x, offset.y, offset.z);
		position += offset;
	}

	// Move in local space
	void TranslateLocal(glm::vec3 offset)
	{
		position += fw::RotateVector(offset, rotation);
		pos_glm.x = position.x;
		pos_glm.y = position.y;
		pos_glm.z = position.z;
	}

	// Move in local space
	void TranslateLocal(fw::Vector3 offset)
	{
		position += fw::RotateVector(offset, rotation);
		pos_glm.x = position.x;
		pos_glm.y = position.y;
		pos_glm.z = position.z;
	}

	// Modify relative rotation in global space
	void Rotate(float ANGLE, fw::Vector3 AXIS);
	// Set absolute position in global space
	void SetPosition(fw::Vector3 POSITION);
	// Set absolute rotation in global space
	void SetRotation(fw::Quaternion ROTATION);
	// Set absolute rotation in 2D space
	void SetRotation(float ROTATION);
	// Set absolute scale in global space
	void SetScale(fw::Vector3 SCALE);

	// Get absolute position in global space
	fw::Vector3 GetPosition();
	// Get absolute rotation in global space
	fw::Quaternion GetRotation();
	// Get absolute scale in global space
	fw::Vector3 GetScale();

	// Get local X vector
	fw::Vector3 GetRight();
	// Get local Y vector
	fw::Vector3 GetUp();
	// Get local Z vector
	fw::Vector3 GetForward();

	// Get GLM matrix
	glm::mat4 getModelMatrix();
};