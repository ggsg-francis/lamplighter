#ifndef INDEX_H
#define INDEX_H

#include "memoryC.h"

#ifdef __cplusplus
extern "C" {
	#endif

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
	} Projectile;

	btID IndexSpawnProjectile();
	void IndexDestroyProjectileC(btID id);

	void IndexDrawTemp();

	extern ObjBufCP block_proj; // Projectile buffer
	extern Projectile proj[BUF_SIZE];

	#ifdef __cplusplus
}
#endif

#endif // END OF FILE
