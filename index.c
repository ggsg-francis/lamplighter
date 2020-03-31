#include "index.h"

#define INLINE __forceinline

//________________________________________________________________________________________________________________________________
// TEMP MATHS --------------------------------------------------------------------------------------------------------------------

// temp
#define CONV_RAD 0.01745329251994329576923690768489
#define CONV_DEG 57.295779513082320876798154814105

#define NULL 0

//(copied from std)
// Maximum value that can be returned by the rand function:
#define RAND_MAX 0x7fff

btf32 Random(btf32 min, btf32 max)
{
	//return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	return min + (btf32)(rand()) / (((btf32)RAND_MAX) / (max - min));
};
inline btf32 Radians(btf32 value)
{
	return value * CONV_RAD;
}
inline btf32 Degrees(btf32 value)
{
	return value * CONV_DEG;
}

void AngToVec2(btf32 angle, btf32* out_x, btf32* out_y)
{
	*out_x = sin(angle);
	*out_y = cos(angle);
}

typedef struct vec2
{
	btf32 x, y;
} vec2;
typedef struct vec3
{
	btf32 x, y, z;
} vec3;

vec3 Vec3Add(vec3 a, vec3 b)
{
	vec3 c;
	c.x = a.x + a.x; c.y = a.y + a.y; c.z = a.z + a.z;
	return c;
}
vec3 Vec3Sub(vec3 a, vec3 b)
{
	vec3 c;
	c.x = a.x - a.x; c.y = a.y - a.y; c.z = a.z - a.z;
	return c;
}
vec3 Vec3Mul(vec3 a, vec3 b)
{
	vec3 c;
	c.x = a.x - a.x; c.y = a.y - a.y; c.z = a.z - a.z;
	return c;
}
vec3 Vec3Div(vec3 a, vec3 b)
{
	vec3 c;
	c.x = a.x - a.x; c.y = a.y - a.y; c.z = a.z - a.z;
	return c;
}

//________________________________________________________________________________________________________________________________
// INDEX -------------------------------------------------------------------------------------------------------------------------

ObjBufCP block_proj; // Projectile buffer
Projectile proj[BUF_SIZE];

void IndexInitialize()
{
	ObjBufCP_init(&block_proj);
}

btID IndexSpawnProjectile()
{
	//yaw += Radians(Random(spread * -0.5f, spread * 0.5f)); // Add horizontal spread
	//pitch += Radians(Random(spread * -0.5f, spread * 0.5f)); // Add vertical spread

	//btID id = block_proj.add();
	return ObjBufCP_add(&block_proj);
}

void IndexDestroyProjectileC(btID id)
{
	//block_proj.remove(id);
	ObjBufCP_remove(&block_proj, id);
}
