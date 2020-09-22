#ifndef MATHS_H
#define MATHS_H

#include "global.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

// For variable argument function!
#include <stdarg.h>

namespace m
{
	//-------------------------------- ANGLE

	#define CONV_RAD 0.01745329251994329576923690768489
	#define CONV_DEG 57.295779513082320876798154814105

	#define IS_ODD_NUMBER(a) (a & 0b1 > 0)

	class Angle { // all calculations done in degrees
	private:
		btf32 deg;
	public:
		Angle() {
			deg = 0.f;
		};
		Angle(btf32 angle) {
			Set(angle);
		};
		inline void Set(btf32 other) {
			deg = other;
			if (deg < 0.f)
				deg += 360.f;
			else if (deg >= 360.f)
				deg -= 360.f;
		}
		inline void Rotate(btf32 other) {
			Set(deg + other);
		}
		inline void RotateClamped(btf32 other, btf32 min, btf32 max) {
			deg += other;
			if (deg < min)
				deg = min;
			else if (deg > max)
				deg = max;
		}
		inline void RotateTowards(btf32 other, btf32 amt) {
			// Sometimes the angles are jacked and stacked, so put them within range
			while (other < 0.f)
				other += 360.f;
			while (other >= 360.f)
				other -= 360.f;
			// why doesn't this work
			//other = fmodf(other, 360.f);
			// Is there any way to optimize this?
			if (fabsf(other - deg) <= amt) // are we close enough to just set the angle
				Set(other);
			else if (fabsf(other - deg) < 180.f) // If we don't have to cross the seam
			{
				if (other > deg)
					Rotate(amt);
				else if (other < deg)
					Rotate(-amt);
			}
			else // Otherwise, we have to cross the seam
			{
				if (other < deg)
					Rotate(amt);
				else if (other > deg)
					Rotate(-amt);
			}
		}
		inline btf32 Deg() {
			return deg;
		}
		inline btf32 Rad() {
			return deg * (btf32)CONV_RAD;
		}
		inline btf32 GetDifference(btf32 other) {
			//Set(deg + other);
			return fabsf(deg - other);
		}
	};

	//#undef CONV_RAD
	//#undef CONV_DEG

	//-------------------------------- VECTOR2
	
	// 2 dimensional vector used for stuff like mouse delta
	class Vector2 {
	public:
		//-------------------------------- VARIABLES
		btf32 x, y;
		//-------------------------------- CONSTRUCTOR
		Vector2(btf32 X = 0.f, btf32 Y = 0.f) : x{ X }, y{ Y } {};
		Vector2(WCoord WC) : x{ (btf32)WC.x }, y{ (btf32)WC.y } {};
		//-------------------------------- OPERATORS
		Vector2 operator+(Vector2);
		Vector2 operator-(Vector2);
		Vector2 operator*(Vector2);
		Vector2 operator/(Vector2);
		Vector2 operator+=(Vector2);
		Vector2 operator-=(Vector2);
		Vector2 operator*=(Vector2);
		Vector2 operator/=(Vector2);
		Vector2 operator+(btf32);
		Vector2 operator-(btf32);
		Vector2 operator*(btf32);
		Vector2 operator/(btf32);
		Vector2 operator+=(btf32);
		Vector2 operator-=(btf32);
		Vector2 operator*=(btf32);
		Vector2 operator/=(btf32);
		Vector2 operator=(const btf32&);
		bool operator==(const Vector2);
	};

	//-------------------------------- VECTOR3
	
	// Class type representing a normal or 3D co-ordinates
	class Vector3 {
	public:
		//-------------------------------- VARIABLES
		btf32 x, y, z;
		//-------------------------------- CONSTRUCTORS
		Vector3() : x{ 0.f }, y{ 0.f }, z{ 0.f } {};
		Vector3(btf32 F) : x{ F }, y{ F }, z{ F } {};
		Vector3(btf32 X, btf32 Y, btf32 Z) : x{ X }, y{ Y }, z{ Z } {};
		Vector3(glm::vec3 V) : x{ V.x }, y{ V.y }, z{ V.z } {};
		//-------------------------------- OPERATORS
		Vector3 operator+(const Vector3& VECTOR);
		Vector3 operator-(const Vector3& VECTOR);
		Vector3 operator*(const btf32 FLOAT);
		Vector3 operator+=(const Vector3& VECTOR);
		Vector3 operator-=(const Vector3& VECTOR);
		Vector3 operator=(const glm::vec3& VECTOR); // GLM to Me
		Vector3 operator=(const glm::vec4& VECTOR); // GLM to Me

		explicit operator glm::vec3() const { return glm::vec3(x, y, z); } // GLM Cast
	};

	Vector3 operator+(const Vector3& VECTOR_A, const Vector3& VECTOR_B);
	Vector3 operator-(const Vector3& VECTOR_A, const Vector3& VECTOR_B);
	Vector3 operator*(const Vector3& VECTOR_A, const Vector3& VECTOR_B);
	Vector3 operator/(const Vector3& VECTOR_A, const Vector3& VECTOR_B);
	Vector3 operator*(const btf32 FLOAT, const Vector3& VECTOR);
	Vector3 operator/(const btf32 FLOAT, const Vector3& VECTOR);

	glm::vec3 operator+(const glm::vec3& VECTOR_A, const Vector3& VECTOR_B);
	glm::vec3 operator*(const glm::vec3& VECTOR_A, const Vector3& VECTOR_B);

	//-------------------------------- VECTOR4
	
	// yeah
	class Vector4 {
	public:
		//-------------------------------- VARIABLES
		btf32 x, y, z, w;
		//-------------------------------- CONSTRUCTORS
		Vector4() : x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 0.f } {};
		Vector4(btf32 F) : x{ F }, y{ F }, z{ F }, w{ F } {};
		Vector4(btf32 X, btf32 Y, btf32 Z, btf32 W) : x{ X }, y{ Y }, z{ Z }, w{ W } {};
		Vector4(glm::vec4 V) : x{ V.x }, y{ V.y }, z{ V.z }, w{ V.w } {};
	};

	//-------------------------------- QUATERNION
	
	// Class type for handling rotations that I don't understand
	class Quaternion {
	public:
		//-------------------------------- VARIABLES
		btf32 x, y, z, w;
		//-------------------------------- CONSTRUCTORS
		Quaternion(btf32 X = 0.f, btf32 Y = 0.f, btf32 Z = 0.f, btf32 W = 1.f) : x{ X }, y{ Y }, z{ Z }, w{ W } {};
		Quaternion(glm::quat Q) : x{ Q.x }, y{ Q.y }, z{ Q.z }, w{ Q.w } {};
		//-------------------------------- OPERATORS
		Quaternion operator*(const Quaternion& QUATERNION);
		// Stub
	};

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//------------- UTILITY FUNCTIONS --------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	char* ToString(int num, char* str, int base);

	btf32 StepToward(btf32 a, btf32 b, btf32 t);
	bti32 StepToward(bti32 a, bti32 b, bti32 t);

	// The "smooth" value is between 0-1, and it represents how much of the
	// original value remains after one second passes
	// Smooth 0 means instant snap, 1 means never move at all
	btf32 BlendToward(btf32 value, btf32 target, btf32 smooth, btf32 delta_time);
	// The "smooth" value is between 0-1, and it represents how much of the
	// original value remains after one second passes
	// Smooth 0 means instant snap, 1 means never move at all
	Vector2 BlendToward(Vector2 value, Vector2 target, btf32 smooth, btf32 delta_time);
	// The "smooth" value is between 0-1, and it represents how much of the
	// original value remains after one second passes
	// Smooth 0 means instant snap, 1 means never move at all
	Vector3 BlendToward(Vector3 value, Vector3 target, btf32 smooth, btf32 delta_time);

	// Linear interpolate
	btf32 Lerp(btf32 a, btf32 b, btf32 t);
	// Linear interpolate Vector2
	Vector2 Lerp(Vector2 vector_a, Vector2 vector_b, btf32 t);
	// Linear interpolate Vector3
	Vector3 Lerp(Vector3 vector_a, Vector3 vector_b, btf32 t);

	// Interpolate towards a value, using a spring
	void SpringDamper(btf32& out_value, btf32& out_velocity, btf32 target_value, btf32 mass, btf32 spring, btf32 damping);
	template <class type> void SpringDamper2(type& out_value, type& out_velocity, type target_value) {
		const btf32 mass = 1 / 3;
		const btf32 timeStep = 0.28;
		const btf32 k = 2;
		const btf32 damping = 10;

		type springForceY = -k*(out_value - target_value);
		type dampingForceY = damping * out_velocity;
		//btf32 forceY = springForceY + mass - dampingForceY;
		type forceY = springForceY - dampingForceY;
		//type accelerationY = forceY / mass;
		type accelerationY = forceY * mass;
		out_velocity += accelerationY * timeStep;
		out_value += out_velocity * timeStep;
	}

	// Get magnitude along a particular plane
	btf32 Dot(const Vector2& veca, const Vector2& vecb);
	// Get magnitude along a particular plane
	btf32 Dot(const Vector3& veca, const Vector3& vecb);
	// ?????????????????????????????????
	btf32 Dot(const Quaternion& quata, const Quaternion& quatb);

	// I can't remember what cross does
	double Cross(Vector2 vector_a, Vector2 vector_b);
	// I can't remember what cross does
	Vector3 Cross(const Vector3& vector_a, const Vector3& vector_b);
	// Cross product function for quaternions
	Quaternion Cross(const Quaternion& quaternion_a, const Quaternion& quaternion_b);

	// Get magnitude of a vector
	btf32 Length(const Vector2& vector);
	// Get magnitude of a vector
	btf32 Length(const Vector3& vector);

	// Convert vector to a magnitude of one
	Vector2 Normalize(const Vector2& vector);
	// Convert vector to a magnitude of one
	Vector3 Normalize(const Vector3& vector);
	// Convert to a magnitude of one??????????
	Quaternion Normalize(const Quaternion& quaternion);

	// Signed difference between two angles in degrees
	btf32 AngDif(btf32 angle_a, btf32 angle_b);
	// Absolute difference between two angles in degrees (Sometimes you only need an absolute value so use this, its quicker)
	btf32 AngDifAbs(btf32 angle_a, btf32 angle_b);
	
	// Take slope values and return an upwards-pointing normal
	// This method of finding Y assumes the normal is a unit vector
	// Basically, its a Length() function but solved for Y where return = 1
	Vector3 NormalFromSlope(const Vector2& slope);

	// Convert radians angle to Vector2
	Vector2 AngToVec2(btf32 angle);
	// Convert radians angle to Vector2 using right-handed rule
	Vector2 AngToVec2RH(btf32 angle);
	// Convert Vector2 to radians angle
	btf32 Vec2ToAng(Vector2 vector);
	// Convert Vector2 to radians angle using right-handed rule
	btf32 Vec2ToAngRH(Vector2 vector);

	// Get value for drawing blend mesh from A to B
	btf32 BlendValueFromDistance(const Vector3& vector_src, const Vector3& vector_dst, btf32 min_length, btf32 max_length);

	// Create quaternion from axis and angle (does not work)
	Quaternion QuatFromAxisAngle(Vector3& axis, btf32 angle);

	// Random btf32 between min and max
	btf32 Random(btf32 min, btf32 max);
	//
	btf32 Clamp(btf32 val, btf32 min, btf32 max);
	//
	//btf32 MaxF(btui32 num, ...);
	template <typename T> T Max(btui32 num, ...) {
		va_list args;
		va_start(args, num);
		T max, get;
		max = va_arg(args, T);
		for (btui32 x = 1; x < num; x++) {
			get = va_arg(args, T);
			if (get > max) max = get;
		}
		va_end(args);
		return max;
	}
	//
	template <typename T> T Min(btui32 num, ...) {
		va_list args;
		va_start(args, num);
		T min, get;
		min = va_arg(args, T);
		for (btui32 x = 1; x < num; x++) {
			get = va_arg(args, T);
			if (get < min) min = get;
		}
		va_end(args);
		return min;
	}
	btf32 Min2(btf32 a, btf32 b);
	btf32 Max2(btf32 a, btf32 b);
	btf32 Min3(btf32 a, btf32 b, btf32 c);
	btf32 Max3(btf32 a, btf32 b, btf32 c);
	btui32 MinIndex(btui32 size, btf32* array);
	btui32 MaxIndex(btui32 size, btf32* array);

	// Quadratic function -- Makes a parabola
	btf32 Quadratic(const btf32 a, const btf32 b, const btf32 c, const btf32 x);
	// Quadratic function optimized for handling character footsteps
	btf32 QuadraticFootstep(const btf32 step_height, const btf32 x);

	// Get angle between two vector2s
	btf32 Vec2Angle(const Vector2& veca, const Vector2& vecb);
	// Rotate a 2D vector by angle
	Vector2 Rotate(Vector2& vector, btf32 angle);

	// Rotate a vector by a quaternion
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
	// Rotate a quaternion around an axis by so many degrees
	Quaternion Rotate(const Quaternion& quaternion, const btf32& angle, const Vector3& axis);
}

#endif // !MATHS_H
