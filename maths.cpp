#include "maths.hpp"
#include <math.h>
//for glm quat (why??)
#include <glm\gtc\type_ptr.hpp>

//// get height offset maths
//float hyp1 = 1.f;
////hyp1 = 4.f;
//float adj1 = 0.75f;
////adj1 = 3.f;
//float ang1 = acos(adj1 / hyp1);
//float ang2 = rad90 - ang1;
//float xhei = hyp1 * cos(ang2);

//float xh2 = cos(asin(3.f / 4.f)) * 4.f;

namespace m
{
	/*
	// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting, 
	// returns t value of intersection and intersection point q 
	int IntersectRaySphere(fw::Vector3 p, fw::Vector3 d, Sphere s, float &t, Point &q)
	{
		fw::Vector3 m = p - s.c;
		float b = fw::Dot(m, d);
		float c = fw::Dot(m, m) - s.r * s.r;

		// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
		if (c > 0.0f && b > 0.0f) return 0;
		float discr = b*b - c;

		// A negative discriminant corresponds to ray missing sphere 
		if (discr < 0.0f) return 0;

		// Ray now found to intersect sphere, compute smallest t value of intersection
		t = -b - sqrt(discr);

		// If t is negative, ray started inside sphere so clamp t to zero 
		if (t < 0.0f) t = 0.0f;
		q = p + t * d;

		return 1;
	}
	*/
}

namespace fw
{
	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| CONSTRUCTORS

	//....................................... VECTOR2

	Vector2::Vector2()
	{
	}

	Vector2::Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2::~Vector2()
	{
	}

	//....................................... VECTOR3

	Vector3::Vector3()
	{
	}
	Vector3::Vector3(float x, float y, float z)
	{
		this->x = x; this->y = y; this->z = z;
	}
	Vector3::Vector3(glm::vec3 v)
	{
		this->x = v.x; this->y = v.y; this->z = v.z;
	}

	//....................................... QUATERNION

	Quaternion::Quaternion()
	{
	}
	Quaternion::Quaternion(float x, float y, float z, float w)
	{
		this->x = x; this->y = y; this->z = z; this->w = w;
	}
	Quaternion::Quaternion(glm::quat q)
	{
		this->w = q.w; this->x = q.x; this->y = q.y; this->z = q.z;
	}

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| MATHS OPERATORS

	//....................................... VECTOR2

	Vector2 Vector2::operator+(Vector2 f)
	{
		Vector2 v(x, y);
		v.x += f.x;
		v.y += f.y;
		return v;
	}

	Vector2 Vector2::operator-(Vector2 f)
	{
		//Vector2 v(x, y);
		//v.x -= f.x;
		//v.y -= f.y;
		//return v;
		return Vector2(x - f.x, y - f.y);
	}

	Vector2 Vector2::operator*(Vector2 f)
	{
		Vector2 v(x, y);
		v.x *= f.x;
		v.y *= f.y;
		return v;
	}

	Vector2 Vector2::operator/(Vector2 f)
	{
		Vector2 v(x, y);
		v.x /= f.x;
		v.y /= f.y;
		return v;
	}

	Vector2 Vector2::operator+(float f)
	{
		Vector2 v(x, y);
		v.x += f;
		v.y += f;
		return v;
	}

	Vector2 Vector2::operator-(float f)
	{
		Vector2 v(x, y);
		v.x -= f;
		v.y -= f;
		return v;
	}

	Vector2 Vector2::operator*(float f)
	{
		Vector2 v(x, y);
		v.x *= f;
		v.y *= f;
		return v;
	}

	Vector2 Vector2::operator/(float f)
	{
		Vector2 v(x, y);
		v.x /= f;
		v.y /= f;
		return v;
	}

	Vector2 Vector2::operator*=(float f)
	{
		return Vector2(x *= f, y *= f);
	}

	Vector2 Vector2::operator/=(float f)
	{
		return Vector2(x /= f, y /= f);
	}

	Vector2 Vector2::operator*(double f)
	{
		Vector2 v(x, y);
		v.x *= f;
		v.y *= f;
		return v;
	}
	Vector2 Vector2::operator+=(Vector2 f) { return Vector2(x += f.x, y += f.y); }

	Vector2 Vector2::operator-=(Vector2 f)
	{
		return Vector2(x -= f.x, y -= f.y);
	}

	Vector2 Vector2::operator*=(Vector2 f)
	{
		return Vector2(x *= f.x, y *= f.y);
	}

	Vector2 Vector2::operator/=(Vector2 f)
	{
		return Vector2(x /= f.x, y /= f.y);
	}

	//....................................... VECTOR3

	Vector3 Vector3::operator*(const float f) { return Vector3(x * f, y * f, z * f); }
	Vector3 Vector3::operator*(const Vector3 & v) { return Vector3(x * v.x, y * v.y, z * v.z); }
	Vector3 Vector3::operator+(const Vector3& v) { return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3 Vector3::operator-(const Vector3& v) { return Vector3(x - v.x, y - v.y, z = v.z); }
	Vector3 Vector3::operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return Vector3(x + v.x, y + v.y, z + z); }
	Vector3 Vector3::operator-=(const Vector3& v) { return Vector3(x -= v.x, y -= v.y, z -= v.z); }
	//Vector3 Vector3::operator=(const Vector3& v) { return v; }
	Vector3 Vector3::operator=(const glm::vec3& v) { return Vector3(v.x, v.y, v.z); }

	//....................................... QUATERNION

	Quaternion Quaternion::operator*(const Quaternion& q) {
		//return Quaternion(x * q.x, y * q.x, z * q.x, w * q.w);

		#define x1 x
		#define y1 y
		#define z1 z
		#define w1 w
		#define x2 q.x
		#define y2 q.y
		#define z2 q.z
		#define w2 q.w

		float xx = (w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2);
		float yy = (w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2);
		float zz = (w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2);
		float ww = (w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2);

		return Quaternion(xx, yy, zz, ww);

		#undef x1
		#undef y1
		#undef z1
		#undef w1
		#undef x2
		#undef y2
		#undef z2
		#undef w2
	}

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| UTILITY FUNCTIONS
	
	//....................................... LERP

	float Lerp(float a, float b, float t)
	{
		return (1 - t)*a + t*b;
	}

	Vector2 Lerp(Vector2 a, Vector2 b, float t)
	{
		return a*(1 - t) + b*t;
	}

	Vector3 Lerp(Vector3 a, Vector3 b, float t)
	{
		//return a*(1 - t) + b*t;
		Vector3 v;
		v.x = a.x * (1 - t) + b.x * t;
		v.y = a.y * (1 - t) + b.y * t;
		v.z = a.z * (1 - t) + b.z * t;
		return v;
	}

	//....................................... VECTOR2

	double Cross(Vector2 a, Vector2 b)
	{
		return a.x * b.y - a.y * b.x;
	}

	/* 'official' maths
	Angle -> Vector2
		float myAngleInRadians = Math.PI;
		Vector2 angleVector = new Vector2(
		(float)Math.Cos(myAngleInRadians),
		-(float)Math.Sin(myAngleInRadians));

	Vector2 -> Angle
		Vector2 angleVector = new Vector2(0, 1);
		float angleFromVector =
		(float)Math.Atan2(angleVector.X, -angleVector.Y);
	*/

	Vector2 AngToVec2(float angle)
	{
		//maybe reversed is correct
		return Vector2(sin(angle), cos(angle));
	}
	Vector2 AngToVec2Correct(float angle)
	{
		return Vector2(cos(angle), sin(angle));
	}

	float Vec2ToAng(Vector2 vec)
	{
		//APPARENTLY x and y are backwards in use here but i dont buy it
		//return (float)atan2(vec.x, -vec.y);
		if (vec.x > 0.f)
			return (float)acos(vec.y);
		else
			return -(float)acos(vec.y);
		//'real' version
		//if (vec.y > 0.f)
		//	return (float)acos(vec.x);
		//else
		//	return -(float)acos(vec.x);
	}
	float Vec2ToAngCorrect(Vector2 vec)
	{
		//APPARENTLY x and y are backwards in use here but i dont buy it
		//return (float)atan2(vec.x, -vec.y);
		//if (vec.x > 0.f)
		//	return (float)acos(vec.y);
		//else
		//	return -(float)acos(vec.y);
		//'real' version
		if (vec.y > 0.f)
			return (float)acos(vec.x);
		else
			return -(float)acos(vec.x);
	}

	float Length(const Vector2& v)
	{
		//return the square root of all axes squared
		return sqrt(v.x * v.x + v.y * v.y);
	}

	Vector2 Normalize(const Vector2 & v)
	{
		Vector2 vector;
		float length = Length(v);
		if (length != 0) {
			vector.x = v.x / length;
			vector.y = v.y / length;
			//vector.z = v.z / length;
		}
		return vector;
	}

	float Dot(const Vector2 & va, const Vector2 & vb)
	{
		return va.x * vb.x + va.y * vb.y;
	}

	float Angle(const Vector2 & va, const Vector2 & vb)
	{
		//float dot = va.x*vb.x + va.y*vb.y; // dot product between[va.x, va.y] and [vb.x, vb.y]
		//float det = va.x*vb.y - va.y*vb.x;  // determinant
		//float angle = atan2(det, dot); //  # atan2(y, x) or atan2(sin, cos)

		float dot = Dot(va, vb);
		float angle = atan(dot);

		return angle;
	}

	//wip
	Vector2 Rotate(Vector2& v, float r)
	{
		float theta = r;

		float cs = cos(theta);
		float sn = sin(theta);

		Vector2 v2;

		v2.x = v.x * cs - v.y * sn;
		v2.y = v.x * sn + v.y * cs;

		return v2;
	}

	//....................................... VECTOR3

	float Length(const Vector3& v)
	{
		//return the square root of all axes squared
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}
	
	Vector3 Normalize(const Vector3& v)
	{
		Vector3 vector;
		float length = Length(v);
		if (length != 0) {
			vector.x = v.x / length;
			vector.y = v.y / length;
			vector.z = v.z / length;
		}
		return vector;
	}

	float Dot(const Vector3& va, const Vector3& vb)
	{
		return va.x * vb.x + va.y * vb.y + va.z * vb.z;
	}

	Vector3 Cross(const Vector3& va, const Vector3& vb)
	{
		return Vector3(va.y * vb.z - va.z * vb.y, va.z * vb.x - va.x * vb.z, va.x * vb.y - va.y * vb.x);
	}

	glm::vec3 QtToGlm(const Vector3& v)
	{
		return glm::vec3(v.x, v.y, v.z);
	}

	//....................................... QUATERNION

	//add length function here

	Quaternion Normalize(const Quaternion& q)
	{
		const float f = 1.0f / sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		return Quaternion(q.x * f, q.y * f, q.z * f, q.w * f);
	}

	float Dot(const Quaternion& qa, const Quaternion& qb)
	{
		return qa.x * qb.x + qa.y * qb.y + qa.z * qb.z + qa.w * qb.w;
	}

	Quaternion Cross(const Quaternion & qa, const Quaternion & qb)
	{
		return Quaternion(	
			qa.w * qb.x + qa.x * qb.w + qa.y * qb.z - qa.z * qb.y,
			qa.w * qb.y + qa.y * qb.w + qa.z * qb.x - qa.x * qb.z,
			qa.w * qb.z + qa.z * qb.w + qa.x * qb.y - qa.y * qb.x,
			qa.w * qb.w - qa.x * qb.x - qa.y * qb.y - qa.z * qb.z);
	}

	Vector3 RotateVector(const Vector3& v, const Quaternion& q)
	{
		const float vx = 2.0f * v.x;
		const float vy = 2.0f * v.y;
		const float vz = 2.0f * v.z;
		const float w2 = q.w * q.w - 0.5f;
		const float dot2 = (q.x * vx + q.y * vy + q.z * vz);
		return Vector3(
			(vx * w2 + (q.y * vz - q.z * vy) * q.w + q.x * dot2),
			(vy * w2 + (q.z * vx - q.x * vz) * q.w + q.y * dot2),
			(vz * w2 + (q.x * vy - q.y * vx) * q.w + q.z * dot2));
	}

	Quaternion Rotate(const Quaternion& q, const float& angle, const Vector3& v)
	{
		Vector3 norm = Normalize(v);
		float const s = sin(angle * 0.5f);
		return Cross(q, Quaternion(norm.x * s, norm.y * s, norm.z * s, cos(angle * 0.5f)));
	}

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| TOXIC SIN ZONE
	
	struct Triangle3D
	{
		Vector3 a;
		Vector3 b;
		Vector3 c;
	};

	//Barycentric to cartesian
	Vector3 BToC3D(Triangle3D tri, Vector3 bar)
	{
		//Vector3 v;
		//v.x = tri.a.x * bar.x + tri.b.x * bar.x + tri.c.x * bar.x;
		//v.y = tri.a.y * bar.y + tri.b.y * bar.y + tri.c.y * bar.y;
		//v.z = tri.a.z * bar.z + tri.b.z * bar.z + tri.c.z * bar.z;
		//return v;

		return tri.a * bar.x + tri.b *  bar.y + tri.c * bar.z;
	}

	//Cartesian to barycentric
	Vector3 CToB3D(Triangle3D tri, Vector3 car)
	{
		Vector3 v;
		v.x = tri.a.x * car.x + tri.b.x * car.x + (1 - tri.a.x - tri.b.x) * car.x;
		return v;
	}

	Quaternion Normalize2(const Quaternion& q)
	{
		//magnitude				=		sqrt(magnitudesquared());
		//magnitudesquared		=		x * x + y * y + z * z + w * w;
		const float f = 1.0f / sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		return Quaternion(q.x * f, q.y * f, q.z * f, q.w * f);
	}

	float Pitch(Quaternion& q)
	{
		//return atan(2.f * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
		
		const float y = 2.f * (q.y * q.z + q.w * q.x);
		const float x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;
		/*
		if (detail::compute_equal<T>::call(y, static_cast<T>(0)) && detail::compute_equal<T>::call(x, static_cast<T>(0))) //avoid atan2(0,0) - handle singularity - Matiis
			return static_cast<T>(static_cast<T>(2) * atan(q.x, q.w));

		return static_cast<T>(atan(y, x));
		*/
		return 0;
	}
	float Yaw(Quaternion& q)
	{
		return 0;
	}
	float Roll(Quaternion& q)
	{
		return 0;
	}
	Vector3 EulerAngles(Quaternion& q)
	{
		return Vector3(0,0,0);
	}
}

//m::Coord2D::Coord2D(int x, int y)
//{
//	this->x = x;
//	this->y = y;
//}