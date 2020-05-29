#ifdef __GNUC__
#include <math.h>
#endif

#include "index.h"

#define INLINE __forceinline

PrjID MakePrjID(int i)
{
	PrjID h;
	h.id = (btID)i;
	return h;
}

//________________________________________________________________________________________________________________________________
// TEMP MATHS --------------------------------------------------------------------------------------------------------------------

// temp
#define CONV_RAD 0.01745329251994329576923690768489
#define CONV_DEG 57.295779513082320876798154814105

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

mem::objbuf_caterpillar block_proj; // Projectile buffer
Projectile proj[BUF_SIZE];

Projectile* GetProj(PrjID id)
{
	return &(proj[id.id]);
}

void IndexInitialize()
{
	//
}

PrjID IndexSpawnProjectile()
{
	return MakePrjID(block_proj.add());
}

void IndexDestroyProjectileC(PrjID id)
{
	block_proj.remove(id.id);
}
