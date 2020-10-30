#pragma once

#include "memory.hpp"
#include "maths.hpp"
#include "env.h" // for cellspace
#include "factions.h"

#include "graphics.hpp" // used by inheritors

//hope these are temporary
#include "glm/glm.hpp"
#include "glm/gtc\matrix_transform.hpp"
#include "glm/gtc\type_ptr.hpp"
//for transform rotation
#include "glm/gtc\quaternion.hpp"

//#include "entity.h" // recursive include allows us to use entity component

//transform
class Transform2D
{
public:
	// world space position
	m::Vector2 position;
	lf32 altitude = 0.f;
	m::Angle yaw;
	CellSpace csi; // Where we are in cell space
};

// Class representing a 3D transformation
class Transform3D
{
protected:
	m::Vector3 position = m::Vector3(0.f, 0.f, 0.f);
	m::Quaternion rotation = m::Quaternion(0.f, 0.f, 0.f, 1.f);
	m::Vector3 scale = m::Vector3(1.f, 1.f, 1.f);

public:
	glm::vec3 pos_glm = glm::vec3(0.f, 0.f, 0.f);
	glm::quat rot_glm = glm::quat(0.f, 0.f, 0.f, 1.f);
	glm::vec3 sca_glm = glm::vec3(1.f, 1.f, 1.f);

	// Move in global space
	void Translate(glm::vec3 offset);
	// Move in global space
	void Translate(m::Vector3 offset);
	// Move in local space
	void TranslateLocal(glm::vec3 offset);
	// Move in local space
	void TranslateLocal(m::Vector3 offset);
	// Modify relative rotation in global space
	void Rotate(float ANGLE, m::Vector3 AXIS);
	// Set absolute position in global space
	void SetPosition(m::Vector3 POSITION);
	// Set absolute rotation in global space
	void SetRotation(m::Quaternion ROTATION);
	// Set absolute rotation in 2D space
	void SetRotation(float ROTATION);
	// Set absolute scale in global space
	void SetScale(m::Vector3 SCALE);

	// Get absolute position in global space
	m::Vector3 GetPosition();
	// Get absolute rotation in global space
	m::Quaternion GetRotation();
	// Get absolute scale in global space
	m::Vector3 GetScale();

	// Get local X vector
	m::Vector3 GetRight();
	// Get local Y vector
	m::Vector3 GetUp();
	// Get local Z vector
	m::Vector3 GetForward();

	// Get GLM matrix
	glm::mat4 getModelMatrix();
	graphics::Matrix4x4 getMatrix();
};

#define STATE_DAMAGE_MAX 1000u

struct MaxedStat
{
private:
	lui16 state = STATE_DAMAGE_MAX;
public:
	//void GetState();
};

enum StatusEffectType : lui16 {
	// HP effects
	EFFECT_DAMAGE_HP,
	EFFECT_RESTORE_HP,
	// Stat effects
	EFFECT_ADD_SPEED,
	EFFECT_RMV_SPEED,
	EFFECT_ADD_AGILITY,
	EFFECT_RMV_AGILITY,
	// Other stuff
	EFFECT_SLOW_TIME,
	EFFECT_BURDEN_ITEM,
	EFFECT_BURN,
	EFFECT_FREEZE,
	EFFECT_TRANSFER_HP,
	EFFECT_LEVITATE,
	EFFECT_WATER_WALK,
	EFFECT_WATER_BREATHE,
	// lunaris' spells
	EFFECT_PARALYZE,
	EFFECT_STUN,
	EFFECT_SLEEP,
	EFFECT_MINDCONTROL,
	EFFECT_CHARM,
	EFFECT_FEAR,
	EFFECT_FRENZY,
	EFFECT_SILENCE,
	EFFECT_BLIND,
	EFFECT_SHIELD,
	// non-spell efect
	EFFECT_LEAD_POISON, // effect gained from eating bullets
};

typedef struct StatusEffect {
	lid effect_caster_id;
	lui16 effect_type;
	lf32 effect_duration;
	lui32 effect_magnitude;
	lid effect_icon;
	lui16 reserved;
} StatusEffect;

// Base entity class
struct ECCommon
{
	li8 name[32];

	// Entity base properties
	enum PhysicsFlags : lui8 {
		// Basic properties
		eCOLLIDE_ENV = 1u, // Handle collision between this entity and the environment
		eCOLLIDE_ENT = 1u << 1u, // Handle collision between this entity and other entities
		eCOLLIDE_MAG = 1u << 3u, // Handle collision between this entity and magic effects
		eCOLLIDE_PRJ = 1u << 2u, // Handle collision between this entity and physical projectiles
		eREPORT_TOUCH = 1u << 4u, // Use callback function when another entity touches this one
		ePHYS_DRAG = 1u << 6u, // Do not tick every frame
		// Can go up to 1 << 7

		ePREFAB_FULLSOLID = eCOLLIDE_ENV | eCOLLIDE_ENT | eCOLLIDE_PRJ | eCOLLIDE_MAG,
		ePREFAB_ITEM = eCOLLIDE_ENV | eCOLLIDE_ENT | ePHYS_DRAG,
	};
	mem::bv<lui8, PhysicsFlags> physicsFlags;
	fac::faction faction;

	lui16 damagestate = STATE_DAMAGE_MAX;
	mem::Buffer32<StatusEffect> effects;
	
	// Global properties, ultimately to be used by every object in the game, incl. environment tiles (wow really huh)
	enum StaticFlags : lui64 {
		eNOTHING = 1u,
		eFLAMMABLE = 1u << 1u,
	};
	mem::bv<lui64, StaticFlags> staticFlags;
	enum ActiveFlags : lui64 {
		eALIVE = 1u,
		eDIED_REPORT = 1u << 1u,
		eGROUNDED = 1u << 2u,
	};
	mem::bv<lui64, ActiveFlags> activeFlags;
	__forceinline bool Grounded() {
		return activeFlags.get(eGROUNDED);
	}

	void Damage(lui32 AMOUNT, lf32 ANGLE);
	void AddEffect(lid CASTER, StatusEffectType TYPE, lf32 DURATION, lui32 MAGNITUDE, lid icon);
	void AddSpell(lid CASTER, lid SPELL);
	void TickEffects(lf32 DELTA_TIME);

	lf32 radius = 0.5f; // Radius of the entity (no larger than .5)
	lf32 height = 1.9f; // Height of the entity cylinder

	Transform2D t;
	
	m::Vector2 velocity;
	lf32 altitude_velocity = 0.f;
	// foot slide for slippery surfaces / knockback etc.
	m::Vector2 slideVelocity;
};

// TODO: belong in some kinda collision file
struct hit_info {
	bool hit = false;
	m::Vector2 surface = m::Vector2(0.f, 0.f);
	m::Vector2 depenetrate = m::Vector2(0.f, 0.f);
	m::Vector2 inheritedVelocity = m::Vector2(0.f, 0.f);
};

// Handle this entitiy's position, receives desired motion
void Entity_PhysicsTick(ECCommon* ENTITY, lid ID, lf32 DELTA_TIME);

char* EntityName(void* ent);
