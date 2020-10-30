/* NOTE ON ANGTOVEC2 / VEC2TOANG

The 'real' maths are as follows

Angle -> Vector2
Vector2 angleVector = Vector2( Cos( myAngleInRadians ), -Sin( myAngleInRadians ) );

Vector2 -> Angle
lf32 angleFromVector = Atan2( angleVector.X, -angleVector.Y );
*/


#include "maths.hpp"
#include <math.h>

namespace m
{
	//-------------------------------- VECTOR2 OPERATORS

	Vector2 Vector2::operator+(Vector2 f) { return Vector2(x + f.x, y + f.y); }
	Vector2 Vector2::operator-(Vector2 f) { return Vector2(x - f.x, y - f.y); }
	Vector2 Vector2::operator*(Vector2 f) { return Vector2(x * f.x, y * f.y); }
	Vector2 Vector2::operator/(Vector2 f) { return Vector2(x / f.x, y / f.y); }

	Vector2 Vector2::operator+=(Vector2 f) { return Vector2(x += f.x, y += f.y); }
	Vector2 Vector2::operator-=(Vector2 f) { return Vector2(x -= f.x, y -= f.y); }
	Vector2 Vector2::operator*=(Vector2 f) { return Vector2(x *= f.x, y *= f.y); }
	Vector2 Vector2::operator/=(Vector2 f) { return Vector2(x /= f.x, y /= f.y); }

	Vector2 Vector2::operator+(lf32 f) { return Vector2(x + f, y + f); }
	Vector2 Vector2::operator-(lf32 f) { return Vector2(x - f, y - f); }
	Vector2 Vector2::operator*(lf32 f) { return Vector2(x * f, y * f); }
	Vector2 Vector2::operator/(lf32 f) { return Vector2(x / f, y / f); }

	Vector2 Vector2::operator+=(lf32 f) { return Vector2(x += f, y += f); }
	Vector2 Vector2::operator-=(lf32 f) { return Vector2(x -= f, y -= f); }
	Vector2 Vector2::operator*=(lf32 f) { return Vector2(x *= f, y *= f); }
	Vector2 Vector2::operator/=(lf32 f) { return Vector2(x /= f, y /= f); }

	Vector2 Vector2::operator=(const lf32& f) { return Vector2(f, f); }

	bool Vector2::operator==(const Vector2 f) { return x == f.x && y == f.y; }

	//-------------------------------- VECTOR3 OPERATORS

	Vector3 Vector3::operator+(const Vector3& v) { return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3 Vector3::operator-(const Vector3& v) { return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3 Vector3::operator*(const lf32 f) { return Vector3(x * f, y * f, z * f); }
	//Vector3 Vector3::operator*(const Vector3& v) { return Vector3(x * v.x, y * v.y, z * v.z); }
	//Vector3 Vector3::operator/(const Vector3& v) { return Vector3(x / v.x, y / v.y, z / v.z); }
	Vector3 Vector3::operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return Vector3(x + v.x, y + v.y, z + z); }
	Vector3 Vector3::operator-=(const Vector3& v) { return Vector3(x -= v.x, y -= v.y, z -= v.z); }
	Vector3 Vector3::operator=(const glm::vec3& v) { return Vector3(v.x, v.y, v.z); }
	Vector3 Vector3::operator=(const glm::vec4& v) { return Vector3(v.x, v.y, v.z); }

	Vector3 operator+(const Vector3 & va, const Vector3 & vb) { return Vector3(va.x + vb.x, va.y + vb.y, va.z + vb.z); }
	Vector3 operator-(const Vector3 & va, const Vector3 & vb) { return Vector3(va.x - vb.x, va.y - vb.y, va.z - vb.z); }
	Vector3 operator*(const Vector3 & va, const Vector3 & vb) { return Vector3(va.x * vb.x, va.y * vb.y, va.z * vb.z); }
	Vector3 operator/(const Vector3 & va, const Vector3 & vb) { return Vector3(va.x / vb.x, va.y / vb.y, va.z / vb.z); }
	Vector3 operator*(const lf32 f, const Vector3& v) { return Vector3(v.x * f, v.y * f, v.z * f); }
	Vector3 operator/(const lf32 f, const Vector3& v) { return Vector3(v.x / f, v.y / f, v.z / f); }

	glm::vec3 operator+(const glm::vec3& va, const Vector3& vb) { return glm::vec3(va.x + vb.x, va.y + vb.y, va.z + vb.z); }
	glm::vec3 operator*(const glm::vec3& va, const Vector3& vb) { return glm::vec3(va.x * vb.x, va.y * vb.y, va.z * vb.z); }

	//-------------------------------- QUATERNION OPERATOR

	Quaternion Quaternion::operator*(const Quaternion& q) {
		return Quaternion
		(
			w * q.x + x * q.w + y * q.z - z * q.y, // XX
			w * q.y - x * q.z + y * q.w + z * q.x, // YY
			w * q.z + x * q.y - y * q.x + z * q.w, // ZZ
			w * q.w - x * q.x - y * q.y - z * q.z  // WW
		);
	}

	//-------------------------------- STEP

	lf32 StepToward(lf32 a, lf32 b, lf32 t) {
		if (b < a - t) return a - t;
		if (b > a + t) return a + t;
		return b;
	}
	li32 StepToward(li32 a, li32 b, li32 t) {
		if (b < a - t) return a - t;
		if (b > a + t) return a + t;
		return b;
	}

	lf32 BlendToward(lf32 value, lf32 target, lf32 smooth, lf32 dt) {
		return Lerp(value, target, 1 - pow(smooth, dt));
	}
	Vector2 BlendToward(Vector2 value, Vector2 target, lf32 smooth, lf32 dt) {
		return Lerp(value, target, 1 - pow(smooth, dt));
	}
	Vector3 BlendToward(Vector3 value, Vector3 target, lf32 smooth, lf32 dt) {
		return Lerp(value, target, 1 - pow(smooth, dt));
	}

	//-------------------------------- LERP

	lf32 Lerp(lf32 a, lf32 b, lf32 t) {
		return (1 - t) * a + t * b;
	}
	Vector2 Lerp(Vector2 a, Vector2 b, lf32 t) {
		return a * (1 - t) + b * t;
	}
	Vector3 Lerp(Vector3 a, Vector3 b, lf32 t) {
		return a * (1 - t) + b * t;
	}

	//-------------------------------- SPRING DAMPER

	void SpringDamper(lf32& out_value, lf32& out_velocity, lf32 target_value, lf32 mass = 30.f, lf32 k = 2.f, lf32 damping = 10.f) {
		//const lf32 mass = 30;
		const lf32 timeStep = 0.28;
		//const lf32 damping = 10;

		/*lf32 springForceY = -k*(out_value - target_value);
		lf32 dampingForceY = damping * out_velocity;
		lf32 forceY = springForceY + mass - dampingForceY;
		lf32 accelerationY = forceY / mass;
		out_velocity += accelerationY * timeStep;
		out_value += out_velocity * timeStep;*/

		lf32 springForceY = -k*(out_value - target_value);
		lf32 dampingForceY = damping * out_velocity;
		//lf32 forceY = springForceY + mass - dampingForceY;
		lf32 forceY = springForceY - dampingForceY;
		lf32 accelerationY = forceY / mass;
		out_velocity += accelerationY * timeStep;
		out_value += out_velocity * timeStep;
	}

	//-------------------------------- DOT

	lf32 Dot(const Vector2 & va, const Vector2 & vb) {
		return va.x * vb.x + va.y * vb.y;
	}
	lf32 Dot(const Vector3& va, const Vector3& vb) {
		return va.x * vb.x + va.y * vb.y + va.z * vb.z;
	}
	lf32 Dot(const Quaternion& qa, const Quaternion& qb) {
		return qa.x * qb.x + qa.y * qb.y + qa.z * qb.z + qa.w * qb.w;
	}

	//-------------------------------- CROSS

	double Cross(Vector2 a, Vector2 b) {
		return a.x * b.y - a.y * b.x;
	}
	Vector3 Cross(const Vector3& va, const Vector3& vb) {
		return Vector3(va.y * vb.z - va.z * vb.y, va.z * vb.x - va.x * vb.z, va.x * vb.y - va.y * vb.x);
	}
	Quaternion Cross(const Quaternion & qa, const Quaternion & qb) {
		return Quaternion(
			qa.w * qb.x + qa.x * qb.w + qa.y * qb.z - qa.z * qb.y,
			qa.w * qb.y + qa.y * qb.w + qa.z * qb.x - qa.x * qb.z,
			qa.w * qb.z + qa.z * qb.w + qa.x * qb.y - qa.y * qb.x,
			qa.w * qb.w - qa.x * qb.x - qa.y * qb.y - qa.z * qb.z);
	}

	//-------------------------------- LENGTH

	lf32 Length(const Vector2& v) {
		//return the square root of all axes squared
		return sqrt(v.x * v.x + v.y * v.y);
	}
	lf32 Length(const Vector3& v) {
		//return the square root of all axes squared
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	//-------------------------------- NORMALIZE

	Vector2 Normalize(const Vector2 & v) {
		Vector2 vector;
		lf32 length = Length(v);
		if (length != 0) {
			vector.x = v.x / length;
			vector.y = v.y / length;
		}
		return vector;
	}
	Vector3 Normalize(const Vector3& v) {
		Vector3 vector;
		lf32 length = Length(v);
		if (length != 0) {
			vector.x = v.x / length;
			vector.y = v.y / length;
			vector.z = v.z / length;
		}
		return vector;
	}
	Quaternion Normalize(const Quaternion& q) {
		const lf32 f = 1.0f / sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		return Quaternion(q.x * f, q.y * f, q.z * f, q.w * f);
	}

	//-------------------------------- ANGLE STUFF

	lf32 AngDif(lf32 anga, lf32 angb) {
		lf32 angdif = angb - anga;
		if (angdif > 180.f)
			angdif -= 360.f;
		else if (angdif < -180.f)
			angdif += 360.f;
		return angdif;
	}
	lf32 AngDifAbs(lf32 anga, lf32 angb) {
		return 180.f - fabsf(fabsf(anga - angb) - 180.f);
	}

	Vector3 NormalFromSlope(const Vector2& slope) {
		return Vector3(-slope.x, sqrt(1.f - (slope.x * slope.x) - (slope.y * slope.y)), -slope.y);
	}

	//-------------------------------- ANGLE / VECTOR2 CONVERSION

	Vector2 AngToVec2(lf32 angle) {
		return Vector2(sin(angle), cos(angle));
	}
	Vector2 AngToVec2RH(lf32 angle) {
		return Vector2(cos(angle), sin(angle));
	}
	lf32 Vec2ToAng(Vector2 vec) {
		if (vec.x > 0.f)
			return (lf32)acos(vec.y);
		else
			return -(lf32)acos(vec.y);
	}
	lf32 Vec2ToAngRH(Vector2 vec) {
		if (vec.y > 0.f)
			return (lf32)acos(vec.x);
		else
			return -(lf32)acos(vec.x);
	}

	//-------------------------------- MISC FUNCTIONS

	lf32 BlendValueFromDistance(const Vector3& src, const Vector3& dst, lf32 min = 0.25f, lf32 max = 1.0f) {
		#define DEADZONE min
		#define MULT (max / (max - DEADZONE))
		return Length(src - dst) * MULT - DEADZONE;
		#undef DEADZONE
		#undef MULT
	}

	/* A utility function to reverse a string  */
	//void reverse(char str[], int length)
	//{
	//	int start = 0;
	//	int end = length - 1;
	//	while (start < end)
	//	{
	//		swap(*(str + start), *(str + end));
	//		start++;
	//		end--;
	//	}
	//}

	// Implementation of itoa() 
	char* ToString(int num, char* str, int base) {
		int i = 0;
		bool isNegative = false;

		/* Handle 0 explicitely, otherwise empty string is printed for 0 */
		if (num == 0)
		{
			str[i++] = '0';
			str[i] = '\0';
			return str;
		}

		// In standard itoa(), negative numbers are handled only with  
		// base 10. Otherwise numbers are considered unsigned. 
		if (num < 0 && base == 10)
		{
			isNegative = true;
			num = -num;
		}

		// Process individual digits 
		while (num != 0)
		{
			int rem = num % base;
			str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
			num = num / base;
		}

		// If number is negative, append '-' 
		if (isNegative)
			str[i++] = '-';

		str[i] = '\0'; // Append string terminator 

		// Reverse the string 
		//reverse(str, i);

		return str;
	}

	/*
	void makeRotationDir(const Vec3& direction, const Vec3& up = Vec3(0, 1, 0)) {
		Vec3 xaxis = Vec3::Cross(up, direction);
		xaxis.normalizeFast();

		Vec3 yaxis = Vec3::Cross(direction, xaxis);
		yaxis.normalizeFast();

		column1.x = xaxis.x;
		column1.y = yaxis.x;
		column1.z = direction.x;

		column2.x = xaxis.y;
		column2.y = yaxis.y;
		column2.z = direction.y;

		column3.x = xaxis.z;
		column3.y = yaxis.z;
		column3.z = direction.z;
	}
	*/

	Quaternion QuatFromAxisAngle(Vector3& axis, lf32 angle) {
		Quaternion q;

		lf32 s = sin(angle / 2);
		q.x = axis.x * s;
		q.y = axis.y * s;
		q.z = axis.z * s;
		q.w = cos(angle / 2);

		return q;
	}

	/*
	Quaternion QuatFromDir(Vector3& dir, Vector3& up) {
		// glm ver.
		//Result[2] = -direction;
		//Result[0] = normalize(cross(up, Result[2]));
		//Result[1] = cross(Result[2], Result[0]);

		Matrix3x3 matr;
		matr.m[2][0] = dir.x;
		matr.m[2][1] = dir.y;
		matr.m[2][2] = dir.z;

		Vector3 side = Cross(dir, up);
		matr.m[0][0] = side.x;
		matr.m[0][1] = side.y;
		matr.m[0][2] = side.z;

		Vector3 un = Cross(dir, side);
		matr.m[1][0] = un.x;
		matr.m[1][1] = un.y;
		matr.m[1][2] = un.z;

		Quaternion q;
		q.w = sqrt(1.0 + matr.m[0][0] + matr.m[1][1] + matr.m[2][2]) / 2.0;
		lf32 w4 = (4.0 * q.w);
		q.x = (matr.m[2][1] - matr.m[1][2]) / w4;
		q.y = (matr.m[0][2] - matr.m[2][0]) / w4;
		q.z = (matr.m[1][0] - matr.m[0][1]) / w4;

		return q;
	}*/

	lf32 Random(lf32 min, lf32 max) {
		return min + static_cast <lf32> (rand()) / (static_cast <lf32> (RAND_MAX / (max - min)));
	}
	lf32 Clamp(lf32 val, lf32 min, lf32 max) {
		if (val < min) return min;
		if (val > max) return max;
		return val;
	}

	lf32 Min2(lf32 a, lf32 b) {
		lf32 min = a;
		if (b < min) min = b;
		return min;
	}
	lf32 Max2(lf32 a, lf32 b) {
		lf32 max = a;
		if (b > max) max = b;
		return max;
	}
	lf32 Min3(lf32 a, lf32 b, lf32 c) {
		lf32 min = a;
		if (b < min) min = b;
		if (c < min) min = c;
		return min;
	}
	lf32 Max3(lf32 a, lf32 b, lf32 c) {
		lf32 max = a;
		if (b > max) max = b;
		if (c > max) max = c;
		return max;
	}
	lui32 MinIndex(lui32 size, lf32* array) {
		lui32 min = 0;
		for (lui32 i = 1; i < size; ++i) {
			if (array[i] < array[min]) min = i;
		}
		return min;
	}
	lui32 MaxIndex(lui32 size, lf32* array) {
		lui32 max = 0;
		for (lui32 i = 1; i < size; ++i) {
			if (array[i] > array[max]) max = i;
		}
		return max;
	}

	lf32 Quadratic(const lf32 a = -1.f, const lf32 b = 0.f, const lf32 c = 1.f, const lf32 x = 0.f) {
		//f(x) = ax 2 + bx + c
		return pow(a*x, 2) + (b * x) + c;
	}
	lf32 QuadraticFootstep(const lf32 step_height = 2.f, const lf32 x = 0.f) {
		// TODO: optimize plz
		return step_height - (pow(-step_height * x, 2)) - (step_height * 0.5f);
	}

	lf32 Vec2Angle(const Vector2& va, const Vector2& vb) {
		//lf32 dot = va.x*vb.x + va.y*vb.y; // dot product between[va.x, va.y] and [vb.x, vb.y]
		//lf32 det = va.x*vb.y - va.y*vb.x;  // determinant
		//lf32 angle = atan2(det, dot); //  # atan2(y, x) or atan2(sin, cos)

		lf32 dot = Dot(va, vb);
		//lf32 angle = atan(dot);
		lf32 angle = acos(dot);

		return angle;
	}

	// WIP
	Vector2 Rotate(Vector2& v, lf32 r) {
		lf32 theta = r;

		lf32 cs = cos(theta);
		lf32 sn = sin(theta);

		Vector2 v2;

		v2.x = v.x * cs - v.y * sn;
		v2.y = v.x * sn + v.y * cs;

		return v2;
	}

	Vector3 RotateVector(const Vector3& v, const Quaternion& q) {
		const lf32 vx = 2.0f * v.x;
		const lf32 vy = 2.0f * v.y;
		const lf32 vz = 2.0f * v.z;
		const lf32 w2 = q.w * q.w - 0.5f;
		const lf32 dot2 = (q.x * vx + q.y * vy + q.z * vz);
		return Vector3(
			(vx * w2 + (q.y * vz - q.z * vy) * q.w + q.x * dot2),
			(vy * w2 + (q.z * vx - q.x * vz) * q.w + q.y * dot2),
			(vz * w2 + (q.x * vy - q.y * vx) * q.w + q.z * dot2));
	}

	Quaternion Rotate(const Quaternion& q, const lf32& angle, const Vector3& v) {
		Vector3 norm = Normalize(v);
		lf32 const s = sin(angle * 0.5f);
		return Cross(q, Quaternion(norm.x * s, norm.y * s, norm.z * s, cos(angle * 0.5f)));
	}

	//-------------------------------- TOXIC SIN ZONE
	
	struct Triangle3D {
		Vector3 a;
		Vector3 b;
		Vector3 c;
	};

	//Barycentric to cartesian
	Vector3 BToC3D(Triangle3D tri, Vector3 bar) {
		//Vector3 v;
		//v.x = tri.a.x * bar.x + tri.b.x * bar.x + tri.c.x * bar.x;
		//v.y = tri.a.y * bar.y + tri.b.y * bar.y + tri.c.y * bar.y;
		//v.z = tri.a.z * bar.z + tri.b.z * bar.z + tri.c.z * bar.z;
		//return v;

		return tri.a * bar.x + tri.b *  bar.y + tri.c * bar.z;
	}

	//Cartesian to barycentric
	Vector3 CToB3D(Triangle3D tri, Vector3 car) {
		Vector3 v;
		v.x = tri.a.x * car.x + tri.b.x * car.x + (1 - tri.a.x - tri.b.x) * car.x;
		return v;
	}

	Quaternion Normalize2(const Quaternion& q) {
		//magnitude				=		sqrt(magnitudesquared());
		//magnitudesquared		=		x * x + y * y + z * z + w * w;
		const lf32 f = 1.0f / sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		return Quaternion(q.x * f, q.y * f, q.z * f, q.w * f);
	}

	lf32 Pitch(Quaternion& q) {
		//return atan(2.f * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
		
		const lf32 y = 2.f * (q.y * q.z + q.w * q.x);
		const lf32 x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;
		/*
		if (detail::compute_equal<T>::call(y, static_cast<T>(0)) && detail::compute_equal<T>::call(x, static_cast<T>(0))) //avoid atan2(0,0) - handle singularity - Matiis
			return static_cast<T>(static_cast<T>(2) * atan(q.x, q.w));

		return static_cast<T>(atan(y, x));
		*/
		return 0;
	}
	lf32 Yaw(Quaternion& q) {
		return 0;
	}
	lf32 Roll(Quaternion& q) {
		return 0;
	}
	Vector3 EulerAngles(Quaternion& q) {
		return Vector3(0,0,0);
	}
}