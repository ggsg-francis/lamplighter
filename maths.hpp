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
		lf32 deg;
	public:
		Angle() {
			deg = 0.f;
		};
		Angle(lf32 angle) {
			Set(angle);
		};
		inline void Set(lf32 other) {
			deg = other;
			if (deg < 0.f)
				deg += 360.f;
			else if (deg >= 360.f)
				deg -= 360.f;
		}
		inline void Rotate(lf32 other) {
			Set(deg + other);
		}
		inline void RotateClamped(lf32 other, lf32 min, lf32 max) {
			deg += other;
			if (deg < min)
				deg = min;
			else if (deg > max)
				deg = max;
		}
		inline void RotateTowards(lf32 other, lf32 amt) {
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
		inline lf32 Deg() {
			return deg;
		}
		inline lf32 Rad() {
			return deg * (lf32)CONV_RAD;
		}
		inline lf32 GetDifference(lf32 other) {
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
		lf32 x, y;
		//-------------------------------- CONSTRUCTOR
		Vector2(lf32 X = 0.f, lf32 Y = 0.f) : x{ X }, y{ Y } {};
		Vector2(WCoord WC) : x{ (lf32)WC.x }, y{ (lf32)WC.y } {};
		//-------------------------------- OPERATORS
		Vector2 operator+(Vector2);
		Vector2 operator-(Vector2);
		Vector2 operator*(Vector2);
		Vector2 operator/(Vector2);
		Vector2 operator+=(Vector2);
		Vector2 operator-=(Vector2);
		Vector2 operator*=(Vector2);
		Vector2 operator/=(Vector2);
		Vector2 operator+(lf32);
		Vector2 operator-(lf32);
		Vector2 operator*(lf32);
		Vector2 operator/(lf32);
		Vector2 operator+=(lf32);
		Vector2 operator-=(lf32);
		Vector2 operator*=(lf32);
		Vector2 operator/=(lf32);
		Vector2 operator=(const lf32&);
		bool operator==(const Vector2);
	};

	//-------------------------------- VECTOR3
	
	// Class type representing a normal or 3D co-ordinates
	class Vector3 {
	public:
		//-------------------------------- VARIABLES
		lf32 x, y, z;
		//-------------------------------- CONSTRUCTORS
		Vector3() : x{ 0.f }, y{ 0.f }, z{ 0.f } {};
		Vector3(lf32 F) : x{ F }, y{ F }, z{ F } {};
		Vector3(lf32 X, lf32 Y, lf32 Z) : x{ X }, y{ Y }, z{ Z } {};
		Vector3(glm::vec3 V) : x{ V.x }, y{ V.y }, z{ V.z } {};
		//-------------------------------- OPERATORS
		Vector3 operator+(const Vector3& VECTOR);
		Vector3 operator-(const Vector3& VECTOR);
		Vector3 operator*(const lf32 FLOAT);
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
	Vector3 operator*(const lf32 FLOAT, const Vector3& VECTOR);
	Vector3 operator/(const lf32 FLOAT, const Vector3& VECTOR);

	glm::vec3 operator+(const glm::vec3& VECTOR_A, const Vector3& VECTOR_B);
	glm::vec3 operator*(const glm::vec3& VECTOR_A, const Vector3& VECTOR_B);

	//-------------------------------- VECTOR4
	
	// yeah
	class Vector4 {
	public:
		//-------------------------------- VARIABLES
		lf32 x, y, z, w;
		//-------------------------------- CONSTRUCTORS
		Vector4() : x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 0.f } {};
		Vector4(lf32 F) : x{ F }, y{ F }, z{ F }, w{ F } {};
		Vector4(lf32 X, lf32 Y, lf32 Z, lf32 W) : x{ X }, y{ Y }, z{ Z }, w{ W } {};
		Vector4(glm::vec4 V) : x{ V.x }, y{ V.y }, z{ V.z }, w{ V.w } {};
	};

	//-------------------------------- QUATERNION
	
	// Class type for handling rotations that I don't understand
	class Quaternion {
	public:
		//-------------------------------- VARIABLES
		lf32 x, y, z, w;
		//-------------------------------- CONSTRUCTORS
		Quaternion(lf32 X = 0.f, lf32 Y = 0.f, lf32 Z = 0.f, lf32 W = 1.f) : x{ X }, y{ Y }, z{ Z }, w{ W } {};
		Quaternion(glm::quat Q) : x{ Q.x }, y{ Q.y }, z{ Q.z }, w{ Q.w } {};
		//-------------------------------- OPERATORS
		Quaternion operator*(const Quaternion& q);
		Quaternion operator*(lf32 f);
		// Stub
	};
	// Probably incorrect implementation
	Quaternion operator*(const lf32 f, const Quaternion& q);
	// Probably incorrect implementation
	Quaternion operator+(const Quaternion& a, const Quaternion& b);

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//------------- UTILITY FUNCTIONS --------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	char* ToString(int num, char* str, int base);

	lf32 StepToward(lf32 a, lf32 b, lf32 t);
	li32 StepToward(li32 a, li32 b, li32 t);

	// The "smooth" value is between 0-1, and it represents how much of the
	// original value remains after one second passes
	// Smooth 0 means instant snap, 1 means never move at all
	lf32 BlendToward(lf32 value, lf32 target, lf32 smooth, lf32 delta_time);
	// The "smooth" value is between 0-1, and it represents how much of the
	// original value remains after one second passes
	// Smooth 0 means instant snap, 1 means never move at all
	Vector2 BlendToward(Vector2 value, Vector2 target, lf32 smooth, lf32 delta_time);
	// The "smooth" value is between 0-1, and it represents how much of the
	// original value remains after one second passes
	// Smooth 0 means instant snap, 1 means never move at all
	Vector3 BlendToward(Vector3 value, Vector3 target, lf32 smooth, lf32 delta_time);

	// Linear interpolate
	lf32 Lerp(lf32 a, lf32 b, lf32 t);
	// Linear interpolate Vector2
	Vector2 Lerp(Vector2 vector_a, Vector2 vector_b, lf32 t);
	// Linear interpolate Vector3
	Vector3 Lerp(Vector3 vector_a, Vector3 vector_b, lf32 t);
	// Spherical Linear interpolate Quaternion
	Quaternion SLerp(Quaternion quat_a, Quaternion quat_b, lf32 t);

	// Interpolate towards a value, using a spring
	void SpringDamper(lf32& out_value, lf32& out_velocity, lf32 target_value, lf32 mass, lf32 spring, lf32 damping);
	template <class type> void SpringDamper2(type& out_value, type& out_velocity, type target_value) {
		const lf32 mass = 1 / 3;
		const lf32 timeStep = 0.28;
		const lf32 k = 2;
		const lf32 damping = 10;

		type springForceY = -k*(out_value - target_value);
		type dampingForceY = damping * out_velocity;
		//lf32 forceY = springForceY + mass - dampingForceY;
		type forceY = springForceY - dampingForceY;
		//type accelerationY = forceY / mass;
		type accelerationY = forceY * mass;
		out_velocity += accelerationY * timeStep;
		out_value += out_velocity * timeStep;
	}

	// Get magnitude along a particular plane
	lf32 Dot(const Vector2& veca, const Vector2& vecb);
	// Get magnitude along a particular plane
	lf32 Dot(const Vector3& veca, const Vector3& vecb);
	// ?????????????????????????????????
	lf32 Dot(const Quaternion& quata, const Quaternion& quatb);

	// I can't remember what cross does
	double Cross(Vector2 vector_a, Vector2 vector_b);
	// I can't remember what cross does
	Vector3 Cross(const Vector3& vector_a, const Vector3& vector_b);
	// Cross product function for quaternions
	Quaternion Cross(const Quaternion& quaternion_a, const Quaternion& quaternion_b);

	// Get magnitude of a vector
	lf32 Length(const Vector2& vector);
	// Get magnitude of a vector
	lf32 Length(const Vector3& vector);

	// Convert vector to a magnitude of one
	Vector2 Normalize(const Vector2& vector);
	// Convert vector to a magnitude of one
	Vector3 Normalize(const Vector3& vector);
	// Convert to a magnitude of one??????????
	Quaternion Normalize(const Quaternion& quaternion);

	// Signed difference between two angles in degrees
	lf32 AngDif(lf32 angle_a, lf32 angle_b);
	// Absolute difference between two angles in degrees (Sometimes you only need an absolute value so use this, its quicker)
	lf32 AngDifAbs(lf32 angle_a, lf32 angle_b);
	
	// Take slope values and return an upwards-pointing normal
	// This method of finding Y assumes the normal is a unit vector
	// Basically, its a Length() function but solved for Y where return = 1
	Vector3 NormalFromSlope(const Vector2& slope);

	// Convert radians angle to Vector2
	Vector2 AngToVec2(lf32 angle);
	// Convert radians angle to Vector2 using right-handed rule
	Vector2 AngToVec2RH(lf32 angle);
	// Convert Vector2 to radians angle
	lf32 Vec2ToAng(Vector2 vector);
	// Convert Vector2 to radians angle using right-handed rule
	lf32 Vec2ToAngRH(Vector2 vector);

	// Get value for drawing blend mesh from A to B
	lf32 BlendValueFromDistance(const Vector3& vector_src, const Vector3& vector_dst, lf32 min_length, lf32 max_length);

	// Create quaternion from axis and angle (does not work)
	Quaternion QuatFromAxisAngle(Vector3& axis, lf32 angle);

	// Random lf32 between min and max
	lf32 Random(lf32 min, lf32 max);
	//
	lf32 Clamp(lf32 val, lf32 min, lf32 max);
	//
	//lf32 MaxF(lui32 num, ...);
	template <typename T> T Max(lui32 num, ...) {
		va_list args;
		va_start(args, num);
		T max, get;
		max = va_arg(args, T);
		for (lui32 x = 1; x < num; x++) {
			get = va_arg(args, T);
			if (get > max) max = get;
		}
		va_end(args);
		return max;
	}
	//
	template <typename T> T Min(lui32 num, ...) {
		va_list args;
		va_start(args, num);
		T min, get;
		min = va_arg(args, T);
		for (lui32 x = 1; x < num; x++) {
			get = va_arg(args, T);
			if (get < min) min = get;
		}
		va_end(args);
		return min;
	}
	lf32 Min2(lf32 a, lf32 b);
	lf32 Max2(lf32 a, lf32 b);
	lf32 Min3(lf32 a, lf32 b, lf32 c);
	lf32 Max3(lf32 a, lf32 b, lf32 c);
	lui32 MinIndex(lui32 size, lf32* array);
	lui32 MaxIndex(lui32 size, lf32* array);

	// Quadratic function -- Makes a parabola
	lf32 Quadratic(const lf32 a, const lf32 b, const lf32 c, const lf32 x);
	// Quadratic function optimized for handling character footsteps
	lf32 QuadraticFootstep(const lf32 step_height, const lf32 x);

	// Get angle between two vector2s
	lf32 Vec2Angle(const Vector2& veca, const Vector2& vecb);
	// Rotate a 2D vector by angle
	Vector2 Rotate(Vector2& vector, lf32 angle);

	// Rotate a vector by a quaternion
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
	// Rotate a quaternion around an axis by so many degrees
	Quaternion Rotate(const Quaternion& quaternion, const lf32& angle, const Vector3& axis);
}

#endif // !MATHS_H
