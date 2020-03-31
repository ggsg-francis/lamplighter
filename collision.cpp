#include "collision.h"

//physx
#include "PxPhysicsAPI.h"

#include "graphics.hpp"

#include "core.h"
#include "objects_entities.h"

using namespace physx;

PxFoundation* pxFoundation;
PxPhysics* pxPhysics;
PxCooking* pxCooking;

//objects that needed to be used outside of main
PxTriangleMesh* pxMTerrain;

void CollisionInit()
{
	//error callback for errors
	//allocator callback is what physx uses to perform memory allocation
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;
	//'Every PhysX module requires a PxFoundation instance to be available.'
	pxFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	//check error
	if (!pxFoundation)
		std::cout << "ERROR: PxCreateFoundation failed!" << std::endl;

	bool recordMemoryAllocations = true;
	//create physics WITHOUT pvd
	//we don't want to use the pvd because it uses the internet
	//pass a default tolerances scale for now
	pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale(), recordMemoryAllocations);
	if (!pxPhysics)
		std::cout << "ERROR: PxCreatePhysics failed!" << std::endl;

	pxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *pxFoundation, PxCookingParams(PxTolerancesScale()));
	if (!pxCooking)
		std::cout << "ERROR: PxCreateCooking failed!" << std::endl;
}

void CollisionEnd()
{
	pxMTerrain->release();
	//release physx stuff from memory
	pxCooking->release();
	pxPhysics->release();
	pxFoundation->release();
}

void CollisionMakeEnvMesh(graphics::TerrainMesh* mesh)
{
	//create physic mesh
	btf32* posbuf = mesh->GenVertexPositionBuffer();

	btf32 p2test[16384];
	memcpy(p2test, posbuf, 16384 * sizeof(btf32));

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = mesh->GetVertexCount();
	meshDesc.points.stride = sizeof(btf32) * 3;
	meshDesc.points.data = posbuf;

	meshDesc.triangles.count = mesh->GetIndexCount() / 3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = mesh->GetIndices();

	meshDesc.flags.set(physx::PxMeshFlag::eFLIPNORMALS);

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	//bool status = mCooking->cookTriangleMesh(meshDesc, writeBuffer, result);
	bool status = pxCooking->cookTriangleMesh(meshDesc, writeBuffer);
	if (!status)
		std::cout << "Cooking mesh failed!" << std::endl;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	pxMTerrain = pxPhysics->createTriangleMesh(readBuffer);

	free(posbuf);

}

void CollideEntity(btID ent)
{
	Entity* entity = ENTITY(ent);

	PxVec3 dir;
	PxReal depth;

	PxCapsuleGeometry sphere = PxCapsuleGeometry(entity->radius, (entity->height - entity->radius) * 0.5f);
	PxTransform tr1 = PxTransform(PxVec3(entity->t.position.x, entity->t.height + entity->radius, entity->t.position.y));
	PxTransform tr2 = PxTransform(PxVec3(0.f, 0.f, 0.f));

	PxU32 iterator;

	if (PxComputeTriangleMeshPenetration(dir, depth, sphere, tr1, pxMTerrain, tr2, 4, &iterator))
	{
		entity->t.position.x += dir.x * depth;
		entity->t.position.y += dir.z * depth;
		entity->t.height += dir.y * depth;
	}
}

bool RayEntity(btID ent, btf32 stand_height)
{
	Entity* entity = ENTITY(ent);

	PxVec3 pos(entity->t.position.x, entity->t.height, entity->t.position.y);
	PxTransform tr2 = PxTransform(PxVec3(0.f, 0.f, 0.f));

	PxTriangleMeshGeometry geom = PxTriangleMeshGeometry(pxMTerrain);

	PxRaycastHit hit_info;
	PxHitFlags flags = PxHitFlag::ePOSITION | PxHitFlag::eDISTANCE;
	PxU32 hitcount = PxGeometryQuery::raycast(pos, PxVec3(0.f, -1.f, 0.f), geom, tr2, stand_height + 0.5f, flags, 1, &hit_info);
	if (hitcount > 0u)
	{
		entity->t.height = m::Lerp(entity->t.height, hit_info.position.y + stand_height, 0.1f);
		entity->t.height_velocity = m::Lerp(entity->t.height_velocity, 0.f, 0.9f);
		return true;
	}
	return false;
}

bool RayProjectile(btf32 px, btf32 py, btf32 pz, btf32 dx, btf32 dy, btf32 dz)
{
	// temp translation to vec3
	m::Vector3 vec(dx, dy, dz);
	btf32 len = m::Length(vec);
	vec = m::Normalize(vec);

	PxVec3 pos(px, py, pz);
	PxTransform tr2 = PxTransform(PxVec3(0.f, 0.f, 0.f));

	PxTriangleMeshGeometry geom = PxTriangleMeshGeometry(pxMTerrain);

	PxRaycastHit hit_info;
	PxHitFlags flags = PxHitFlag::eDISTANCE;
	PxU32 hitcount = PxGeometryQuery::raycast(pos, PxVec3(vec.x, vec.y, vec.z), geom, tr2, len, flags, 1, &hit_info);
	if (hitcount > 0u)
	{
		return true;
	}
	return false;
}
