#ifndef INDEX_H
#define INDEX_H

#include "memory.hpp"

typedef struct _prjid { btID id; } PrjID;

PrjID MakePrjID(int i);

void IndexInitialize();

// Transform structure used for game simulation (C version)
typedef struct
{
	// world space position
	btf32 position_x;
	btf32 position_y;
	btf32 position_h;
	btf32 velocity_x;
	btf32 velocity_y;
	btf32 velocity_h;
	btui8 cellx;
	btui8 celly;
} TransformC;

typedef struct
{
	TransformC t;
	btui64 ttd;
	btui32 faction;
	btID type;
} Projectile;

PrjID IndexSpawnProjectile();
void IndexDestroyProjectileC(PrjID id);

Projectile* GetProj(PrjID id);

PrjID ProjFirst();
PrjID ProjLast();

extern mem::objbuf_caterpillar block_proj; // Projectile buffer
extern Projectile proj[BUF_SIZE];

#endif // END OF FILE
