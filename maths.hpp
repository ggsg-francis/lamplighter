#pragma once
#include <glm\glm.hpp>

namespace m
{
	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| ANGLE

	#define CONV_RAD 0.01745329251994329576923690768489
	#define CONV_DEG 57.295779513082320876798154814105

	//inline void Set(btf32 other)
	//{
	//	deg = other;
	//	if (deg < 0.f)
	//		deg = 0.f;
	//	else if (deg >= 360.f)
	//	{
	//		deg -= 360.f;
	//	}
	//}
	//inline void Rotate(btf32 from, btf32 to)
	//{
	//	//Set(deg + other);
	//}

	//temp
	inline btf32 RotateTowards(btf32 from, btf32 to, btf32 amt)
	{
		//start complicated, get simple over time
		if (fabsf(to - from) <= amt) // are we close enough to just set the angle
			return to;
		else if (fabsf(from - to) < 180.f) // no seam
		{
			if (to > from) // otherwise, we have to move closer by amt
			{
				return to + amt;
			}
			else if (to < from)
			{
				return to - amt;
			}
		}
		else // we have to cross the seam
		{
			return to;
		}
	}

	class Angle // all calculations done in degrees?
	{
	private:
		btf32 deg;
	public:
		Angle()
		{
			deg = 0.f;
		};
		Angle(btf32 angle)
		{
			deg = angle;
		};
		inline void Set(btf32 other)
		{
			deg = other;
			if (deg < 0.f)
				deg += 360.f;
			else if (deg >= 360.f)
			{
				deg -= 360.f;
			}
		}
		inline void Rotate(btf32 other)
		{
			Set(deg + other);
		}
		inline void RotateClamped(btf32 other, btf32 min, btf32 max)
		{
			deg += other;
			if (deg < min)
				deg = min;
			else if (deg > max)
				deg = max;
		}
		inline void RotateTowards(btf32 other, btf32 amt)
		{
			//start complicated, get simple over time
			if (fabsf(other - deg) <= amt) // are we close enough to just set the angle
				Set(other);
			// otherwise, we have to move closer by amt
			else if (fabsf(deg - other) < 180.f) // no seam
			{
				if (other > deg)
				{
					Rotate(amt);
				}
				else if (other < deg)
				{
					Rotate(-amt);
				}
			}
			else // we have to cross the seam
			{
				if (other < deg)
				{
					Rotate(amt * 2);
				}
				else if (other > deg)
				{
					Rotate(-amt * 2);
				}
			}
		}
		inline btf32 Deg()
		{
			return deg;
		}
		inline btf32 Rad()
		{
			return deg * (btf32)CONV_RAD;
		}
		inline btf32 GetDifference(btf32 other)
		{
			//Set(deg + other);
			return fabsf(deg - other);
		}
	};

	#undef CONV_RAD
	#undef CONV_DEG
}

namespace fw
{
	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| VECTOR2

	//2 dimensional vector used for stuff like mouse delta
	class Vector2
	{
	public:
		float x, y;
		Vector2();
		Vector2(float X, float Y);
		~Vector2();
		Vector2 operator+(Vector2);
		Vector2 operator-(Vector2);
		Vector2 operator*(Vector2);
		Vector2 operator/(Vector2);

		Vector2 operator+(float);
		Vector2 operator-(float);
		Vector2 operator*(float);
		Vector2 operator/(float);

		Vector2 operator*=(float);
		Vector2 operator/=(float);

		Vector2 operator*(double);
		Vector2 operator+=(Vector2);
		Vector2 operator-=(Vector2);
		Vector2 operator*=(Vector2);
		Vector2 operator/=(Vector2);
	};

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| VECTOR3

	//Class type representing a normal or 3D co-ordinates
	class Vector3
	{
	public:
		//....................................... CONSTRUCTORS
		Vector3();
		Vector3(float X, float Y, float Z);
		//Vector3(physx::PxVec3 VECTOR);
		Vector3(glm::vec3 VECTOR);
		//....................................... OPERATORS
		Vector3 operator*(const float FLOAT);
		Vector3 operator*(const Vector3& VECTOR);
		Vector3 operator+(const Vector3& VECTOR);
		Vector3 operator-(const Vector3& VECTOR);
		Vector3 operator+=(const Vector3& VECTOR);
		Vector3 operator-=(const Vector3& VECTOR);
		//Vector3 operator=(const Vector3& VECTOR);
		//Vector3 operator=(const physx::PxVec3& VECTOR);
		Vector3 operator=(const glm::vec3& VECTOR);
		//....................................... VARIABLES
		float x = 0, y = 0, z = 0;
	};

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| QUATERNION

	//Class type for handling rotations that I don't understand
	class Quaternion
	{
	public:
		//....................................... CONSTRUCTORS
		Quaternion();
		Quaternion(float X, float Y, float Z, float W);
		//Quaternion(physx::PxQuat QUATERNION);
		Quaternion(glm::quat QUATERNION);
		//....................................... OPERATORS
		Quaternion operator*(const Quaternion& QUATERNION);
		//to do
		//....................................... VARIABLES
		float x = 0.f, y = 0.f, z = 0.f, w = 1.f;
	};

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| UTILITY CLASSES

	//make these standard enforced

	class Position_DEPRECATED
	{
	public:
		//height
		float h = 0.f;
		//tile xy
		int tx = 0, ty = 0;
		//tile offset, possibly means x,y vars are useless
		float ox = 0.f, oy = 0.f;
	};

	// too simple?
	class Rotation_TEST
	{
		float h, v;
	};

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| UTILITY FUNCTIONS

	//rotate angle towards
	//if you cant do that, then put a check that says if the number of degrees you need to turn is > 180, store the value as x and then subtract 360 - x and move that many degrees in the other direction

	//....................................... LERP

	float Lerp(float A, float B, float T);
	Vector2 Lerp(Vector2 A, Vector2 B, float T);
	Vector3 Lerp(Vector3 A, Vector3 B, float T);

	//....................................... VECTOR2


	double Cross(Vector2 A, Vector2 B);

	//
	Vector2 AngToVec2(float ANGLE);
	Vector2 AngToVec2Correct(float ANGLE);
	float Vec2ToAng(Vector2 VECTOR);
	float Vec2ToAngCorrect(Vector2 VECTOR);

	float Length(const Vector2& VECTOR);

	//Convert vector to a magnitude of one
	Vector2 Normalize(const Vector2& VECTOR);

	//Get magnitude along a particular plane
	float Dot(const Vector2& VECA, const Vector2& VECB);
	float Angle(const Vector2& VECA, const Vector2& VECB);

	// Rotate a 2D vector by R
	Vector2 Rotate(Vector2& v, float r);

	//....................................... VECTOR3

	//Get magnitude of a vector
	float Length(const Vector3& VECTOR);

	//Convert vector to a magnitude of one
	Vector3 Normalize(const Vector3& VECTOR);

	//Get magnitude along a particular plane
	float Dot(const Vector3& VECA, const Vector3& VECB);

	//i can't remember what cross does
	Vector3 Cross(const Vector3& VECA, const Vector3& VECB);

	//Convert QT vector to GLM vector
	glm::vec3 QtToGlm(const Vector3& VECTOR);

	//....................................... QUATERNION

	//convert to a magnitude of one??????????
	Quaternion Normalize(const Quaternion& QUATERNION);

	//?????????????????????????????????
	float Dot(const Quaternion& QUATA, const Quaternion& QUATB);

	//Cross product function for quaternions
	Quaternion Cross(const Quaternion& QUATA, const Quaternion& QUATB);

	//Rotate a vector by a quaternion
	Vector3 RotateVector(const Vector3& VECTOR, const Quaternion& QUATERNION);

	//Rotate a quaternion around an axis by so many degrees
	Quaternion Rotate(const Quaternion& QUATERNION, const float& ANGLE, const Vector3& AXIS);

	//Get quaternion pitch
	float Pitch(Quaternion& QUAT);
	//Get quaternion yaw
	float Yaw(Quaternion& QUAT);
	//Get quaternion roll
	float Roll(Quaternion& QUAT);

	//Get quaternion rotation as vector3 class
	Vector3 EulerAngles(Quaternion& QUAT);
}