#include "ec_common.h"

#include "archive.hpp"
#include "index.h"
#include "core.h"
#include "audio.hpp"

void Transform3D::Translate(glm::vec3 offset) {
	pos_glm += offset;
	position = pos_glm;
}
void Transform3D::Translate(m::Vector3 offset) {
	pos_glm += glm::vec3(offset.x, offset.y, offset.z);
	position += offset;
}
void Transform3D::TranslateLocal(glm::vec3 offset) {
	position += m::RotateVector(offset, rotation);
	pos_glm.x = position.x;
	pos_glm.y = position.y;
	pos_glm.z = position.z;
}
void Transform3D::TranslateLocal(m::Vector3 offset) {
	position += m::RotateVector(offset, rotation);
	pos_glm.x = position.x;
	pos_glm.y = position.y;
	pos_glm.z = position.z;
}
void Transform3D::Rotate(float angle, m::Vector3 axis) {
	SetRotation(m::Rotate(rotation, angle, axis));
}
void Transform3D::SetPosition(m::Vector3 v) {
	position = v;
	pos_glm.x = v.x;
	pos_glm.y = v.y;
	pos_glm.z = v.z;
}
void Transform3D::SetRotation(m::Quaternion q) {
	rotation = q;
	rot_glm.x = q.x;
	rot_glm.y = q.y;
	rot_glm.z = q.z;
	rot_glm.w = q.w;
}
void Transform3D::SetRotation(float rot) {
	m::Quaternion q = m::Rotate(m::Quaternion(0, 0, 0, 1), rot, m::Vector3(0, 1, 0));
	rotation = q;
	rot_glm.x = q.x;
	rot_glm.y = q.y;
	rot_glm.z = q.z;
	rot_glm.w = q.w;
}
void Transform3D::SetScale(m::Vector3 v) {
	scale = v;
	sca_glm.x = v.x;
	sca_glm.y = v.y;
	sca_glm.z = v.z;
}
m::Vector3 Transform3D::GetPosition() {
	return position;
}
m::Quaternion Transform3D::GetRotation() {
	return rotation;
}
m::Vector3 Transform3D::GetScale() {
	return scale;
}
m::Vector3 Transform3D::GetRight() {
	return m::RotateVector(m::Vector3(1, 0, 0), rotation);
}
m::Vector3 Transform3D::GetUp() {
	return m::RotateVector(m::Vector3(0, 1, 0), rotation);
}
m::Vector3 Transform3D::GetForward() {
	return m::RotateVector(m::Vector3(0, 0, 1), rotation);
}
glm::mat4 Transform3D::getModelMatrix() {
	glm::mat4 model = glm::mat4(1.0f); // Create identity matrix
	model = glm::scale(model, glm::vec3(1.f, 1.f, -1.f)); // See if we can remove this extra transformation!
	model = glm::translate(model, pos_glm);
	model *= glm::mat4_cast(rot_glm);
	model = glm::scale(model, sca_glm);
	return model;
}
graphics::Matrix4x4 Transform3D::getMatrix() {
	glm::mat4 model = glm::mat4(1.0f); // Create identity matrix
	model = glm::scale(model, glm::vec3(1.f, 1.f, -1.f)); // See if we can remove this extra transformation!
	model = glm::translate(model, pos_glm);
	model *= glm::mat4_cast(rot_glm);
	model = glm::scale(model, sca_glm);
	graphics::Matrix4x4 model2;
	model2[0][0] = model[0][0];
	model2[0][1] = model[0][1];
	model2[0][2] = model[0][2];
	model2[0][3] = model[0][3];

	model2[1][0] = model[1][0];
	model2[1][1] = model[1][1];
	model2[1][2] = model[1][2];
	model2[1][3] = model[1][3];

	model2[2][0] = model[2][0];
	model2[2][1] = model[2][1];
	model2[2][2] = model[2][2];
	model2[2][3] = model[2][3];

	model2[3][0] = model[3][0];
	model2[3][1] = model[3][1];
	model2[3][2] = model[3][2];
	model2[3][3] = model[3][3];
	return model2;
}

void ECCommon::Damage(lui32 amount, lf32 angle)
{
	//aud::PlaySnd(aud::FILE_SWING_CONNECT, );
	// if it's enough to reduce us to or below zero
	if (amount >= damagestate) {
		//stateFlags.unset((ActiveFlags)(eALIVE | eDIED_THIS_TICK));
		activeFlags.unset(eALIVE);
		activeFlags.set(eDIED_REPORT);
		damagestate = 0u;
	}
	else {
		damagestate -= amount;
	}
	// TODO: include AI 'notify attack' function call here
}
void ECCommon::AddEffect(LtrID caster, StatusEffectType type, lf32 duration, lui32 magnitude, ID16 icon)
{
	StatusEffect effect;
	effect.effect_caster_id = caster;
	effect.effect_type = type;
	effect.effect_duration = duration;
	effect.effect_magnitude = magnitude;
	effect.effect_icon = icon;
	effects.Add(effect);

	aud::PlaySnd3D(aud::FILE_EFFECT, m::Vector3(ENTITY(caster)->t.position.x, ENTITY(caster)->t.altitude, ENTITY(caster)->t.position.y));

	// TODO: include AI 'notify attack' function call here
}
void ECCommon::AddSpell(LtrID caster, ID16 spell)
{
	AddEffect(caster, (StatusEffectType)acv::spells[spell].target_effect_type,
		acv::spells[spell].target_effect_duration,
		acv::spells[spell].target_effect_magnitude,
		acv::spells[spell].icon);
	// TODO: this only works in the case of cast on self, should deal with this properly but dont know how best to yet
	if (IDCOMPARE(core::players[0], caster)) {
		char string[64] = "Got Effect: ";
		strcat(string, (char*)acv::spells[spell].name);
		core::GUISetMessag(0, string);
	}
	else if (IDCOMPARE(core::players[1], caster)) {
		char string[64] = "Got Effect: ";
		strcat(string, (char*)acv::spells[spell].name);
		core::GUISetMessag(1, string);
	}
}
void ECCommon::TickEffects(lf32 dt)
{
	for (lui32 i = 0; i < effects.Size(); ++i) {
		if (!effects.Used(i)) continue; // Skip unused
		switch (effects[i].effect_type) {
		case EFFECT_DAMAGE_HP:
			Damage(effects[i].effect_magnitude, 0.f);
			break;
		case EFFECT_RESTORE_HP:
			damagestate += (lui16)effects[i].effect_magnitude;
			if (damagestate >= STATE_DAMAGE_MAX) damagestate = STATE_DAMAGE_MAX;
			break;
		}
		effects[i].effect_duration -= dt; // tick down the effect timer
		if (effects[i].effect_duration < 0.f) { // if the timer has run out
			effects.Remove(i); // remove the effect
		}
	}
}

#include "3rdparty\cute_c2.h"

// TODO: temmmmmmmmmmmmmmpppppppp
// temporary because the physics functions really shouldnt
// be using if ACTOR anymore, its jank
#include "ec_actor.h"

#define ACTOR_NO_COLLIDE_HEIGHT 0.0625f

// toggle WIP continuous collision
#define DEF_CCD 0

#if !DEF_GRID
// TODO: same treatment to slidevelocity as regular velocity
void Entity_PhysLineDeintersect(ECCommon* entity, env::EnvLineSeg* seg, lf32 radius)
{
	#if DEF_CCD
	c2Circle circle;
	circle.p = c2V(entity->t.position.x, entity->t.position.y);
	circle.r = entity->radius * 0.5f;
	c2Capsule capsule;
	capsule.a = c2V(seg->pos_a.x, seg->pos_a.y);
	capsule.b = c2V(seg->pos_b.x, seg->pos_b.y);
	capsule.r = circle.r;

	c2x tform1 = c2xIdentity();
	c2v vel1 = c2V(entity->velocity.x, entity->velocity.y);
	c2v vel2 = c2V(0.f, 0.f);

	li32 out_iterations;

	lf32 toi = c2TOI(&circle, C2_TYPE_CIRCLE, &tform1, vel1, &capsule, C2_TYPE_CAPSULE, &tform1, vel2, 0, &out_iterations);

	if (toi == 0.f) {
		lf32 vellen = m::Length(entity->velocity);
		m::Vector2 lineDir = m::Normalize(seg->pos_a - seg->pos_b);
		lf32 slideDir = m::Dot(entity->velocity, lineDir);
		entity->velocity = lineDir * (lf32)(slideDir > 0.f) * vellen;
	}
	else if (toi < 1.f) {
		printf("CCD into wall\n");

		lf32 vellen = m::Length(entity->velocity);

		entity->velocity *= toi;

		m::Vector2 lineDir = m::Normalize(seg->pos_a - seg->pos_b);

		lf32 slideDir = m::Dot(entity->velocity, lineDir);

		entity->velocity += (lineDir * (lf32)(slideDir > 0.f)) * (vellen * (1.f - toi));
	}

	#else

	hit_info hit;
	m::Vector2 offsettemp(0.f, 0.f);
	// Get the offset between us and the line
	m::Vector2 localvec(entity->t.position.x - seg->csn_position.x, entity->t.position.y - seg->csn_position.y);
	// Rotate the offset vector by the negative line rotation so it is axis-aligned
	localvec = m::Rotate(localvec, -seg->csn_rotation);
	// Perform Bounding Box checks from here...
	// If we're touching the wall plane
	if (localvec.y < radius && localvec.y > -radius) {
		// if we're not within the wall space, deal with the end semicircles
		if (localvec.x > seg->csn_scale.x) {
			// Circle collision
			// Are we within this wall's height range?
			if (!((seg->h_b_top <= entity->t.altitude + ACTOR_NO_COLLIDE_HEIGHT && seg->h_b_bot <= entity->t.altitude + entity->height)
				|| (seg->h_b_top >= entity->t.altitude + ACTOR_NO_COLLIDE_HEIGHT && seg->h_b_bot >= entity->t.altitude + entity->height))) {
				m::Vector2 offset = localvec - m::Vector2(seg->csn_scale.x, 0);
				if (m::Length(offset) < radius) {
					hit.hit = true;
					m::Vector2 pushBy = m::Normalize(offset) * (0.5f - m::Length(offset));
					pushBy = m::Rotate(pushBy, seg->csn_rotation);
					offsettemp += pushBy;
					hit.surface = m::Normalize(pushBy);
				}
			}
		}
		else if (localvec.x < -seg->csn_scale.x) {
			// Are we within this wall's height range?
			if (!((seg->h_a_top <= entity->t.altitude + ACTOR_NO_COLLIDE_HEIGHT && seg->h_a_bot <= entity->t.altitude + entity->height)
				|| (seg->h_a_top >= entity->t.altitude + ACTOR_NO_COLLIDE_HEIGHT && seg->h_a_bot >= entity->t.altitude + entity->height))) {
				// Circle collision
				m::Vector2 offset = localvec - m::Vector2(-seg->csn_scale.x, 0);
				if (m::Length(offset) < radius) {
					hit.hit = true;
					m::Vector2 pushBy = m::Normalize(offset) * (0.5f - m::Length(offset));
					pushBy = m::Rotate(pushBy, seg->csn_rotation);
					offsettemp += pushBy;
					hit.surface = m::Normalize(pushBy);
				}
			}
		}
		// If we're within the segment, deal with the straight face
		else {
			lf32 lerpval = ((localvec.x / seg->csn_scale.x) * 0.5f) + 0.5f;
			// MIGHT be backwards
			lf32 height_lerp_top = m::Lerp(seg->h_a_top, seg->h_b_top, lerpval);
			lf32 height_lerp_bot = m::Lerp(seg->h_a_bot, seg->h_b_bot, lerpval);
			// Are we within this wall's height range?
			if (!((height_lerp_top <= entity->t.altitude + ACTOR_NO_COLLIDE_HEIGHT && height_lerp_bot <= entity->t.altitude + entity->height)
				|| (height_lerp_top >= entity->t.altitude + ACTOR_NO_COLLIDE_HEIGHT && height_lerp_bot >= entity->t.altitude + entity->height))) {
				hit.hit = true;
				// If we're on the front face of the wall
				if (localvec.y >= 0) {
					hit.surface = m::AngToVec2RH(seg->csn_rotation + glm::radians(90.f));
					m::Vector2 temp(0.f, 0.f);
					temp.y = radius - localvec.y;
					temp = m::Rotate(temp, seg->csn_rotation);
					offsettemp += temp;
				}
				// Back face
				else {
					hit.surface = m::AngToVec2RH(seg->csn_rotation + glm::radians(-90.f));
					m::Vector2 temp(0.f, 0.f);
					temp.y = localvec.y + radius;
					temp = m::Rotate(temp, seg->csn_rotation);
					offsettemp -= temp;
				}
			}
		}
	}

	hit.depenetrate = offsettemp;

	if (hit.hit) {
		// modify position
		entity->t.position += hit.depenetrate;
		// modify velocity
		lf32 velLen = m::Length(entity->velocity);
		lf32 dir = m::Dot(entity->velocity, m::Normalize(m::Vector2(offsettemp.y, -offsettemp.x)));
		if (m::Length(entity->velocity) > 0.f) {
			#if DEF_WALL_SLIDE_PRESERVE_SPEED
			// Maintain speed along new vector
			entity->velocity = m::Normalize(m::Vector2(offsettemp.y, -offsettemp.x) * dir) * velLen;
			#else
			// Simple velocity reduction
			entity->velocity = m::Normalize(m::Vector2(offsettemp.y, -offsettemp.x)) * dir;
			#endif
		}
		//entity->Collide(hit);
	}

	#endif
}
#endif
void Entity_Collision(LtrID id, ECCommon* ent, CellSpace& csi)
{
	lf32 offsetx, offsety;
	bool overlapN, overlapS, overlapE, overlapW;

	//-------------------------------- ENTITY COLLISION CHECK

	// exploding item
	/*
	if (GetEntityType(id) == ENTITY_TYPE_RESTING_ITEM) {
	RestingItem* item = ITEM(id);
	HeldItem* iteminst = (HeldItem*)GetItemInstance(item->item_instance);
	if (mem::bvget<lui16>(acv::items[iteminst->id_item_template]->bv_base, (lui16)acv::ItemRecord::eDETONATEABLE)) {
	for (btcoord x = csi.c[eCELL_I].x - 1u; x < csi.c[eCELL_I].x + 1u; ++x) {
	for (btcoord y = csi.c[eCELL_I].y - 1u; y < csi.c[eCELL_I].y + 1u; ++y) {
	// De-intersect against other entities
	for (int e = 0; e < core::CellEntityCount(x, y); e++) {
	if (core::CellEntity(x, y, e) != ID_NULL) {
	if (GetEntityExists(core::CellEntity(x, y, e))) {
	m::Vector2 vec = ent->t.position - ENTITY(core::CellEntity(x, y, e))->t.position;
	float dist = m::Length(vec);
	lf32 combined_radius = ent->radius + ENTITY(core::CellEntity(x, y, e))->radius;
	if (dist < combined_radius && dist > 0.f) {
	// explode
	ENTITY(core::CellEntity(x, y, e))->Damage(900, 0.f);
	ent->Damage(1000, 0.f);
	aud::PlaySnd(aud::FILE_SWING_CONNECT, m::Vector3(ent->t.position.x, ent->t.height, ent->t.position.y));
	}
	}
	} // End for each entity in cell
	} // End if entity count of this cell is bigger than zero
	} // End for each cell group Y
	} // End for each cell group X
	}
	}*/

	if (ent->physicsFlags.get(ECCommon::eCOLLIDE_ENT)) {
		for (btcoord x = csi.c[eCELL_I].x - 1u; x < csi.c[eCELL_I].x + 1u; ++x) {
			for (btcoord y = csi.c[eCELL_I].y - 1u; y < csi.c[eCELL_I].y + 1u; ++y) {
				// De-intersect against other entities
				for (int e = 0; e < core::CellEntityCount(x, y); e++) {
					if (IDCHECK(core::CellEntity(x, y, e))) {
						if (GetEntityExists(core::CellEntity(x, y, e)) && ENTITY(core::CellEntity(x, y, e))->physicsFlags.get(ECCommon::eCOLLIDE_ENT)) {
							m::Vector2 vec = ent->t.position - ENTITY(core::CellEntity(x, y, e))->t.position;
							float dist = m::Length(vec);
							lf32 combined_radius = ent->radius + ENTITY(core::CellEntity(x, y, e))->radius;
							if (dist < combined_radius && dist > 0.f) {
								// TEMP! if same type
								if (GetEntityType(id.Index()) == GetEntityType(core::CellEntity(x, y, e).Index())) {
									ent->t.position += m::Normalize(vec) * (combined_radius - dist) * 0.5f;
									ENTITY(core::CellEntity(x, y, e))->t.position -= m::Normalize(vec) * (combined_radius - dist) * 0.5f;
								}
							}
						}
					} // End for each entity in cell
				} // End if entity count of this cell is bigger than zero
			} // End for each cell group Y
		} // End for each cell group X
	} // end does collide entities check

	//-------------------------------- ENVIRONMENTAL COLLISION CHECK (2ND THEREFORE PRIORITIZED)

	#if !DEF_GRID
	// Line test
	WCoord coords;
	for (int cell = 0; cell < eCELL_COUNT; ++cell) {
		coords.x = ent->t.csi.c[cell].x;
		coords.y = ent->t.csi.c[cell].y;
		// check against all triangles
		for (int i = 0; i < env::GetNumLines(coords); ++i) {
			env::EnvLineSeg* seg = env::GetLine(coords, i);
			Entity_PhysLineDeintersect(ent, seg, 0.5f);
		}
	}
	#endif

	#if DEF_GRID
	offsetx = ent->t.position.x - ent->t.csi.c[eCELL_I].x;
	offsety = ent->t.position.y - ent->t.csi.c[eCELL_I].y;

	overlapN = offsety > 0.f;
	overlapS = offsety < 0.f;
	overlapE = offsetx > 0.f;
	overlapW = offsetx < 0.f;


	// STRAIGHT EDGE COLLISION CHECK

	lf32 height_terrain;
	// North
	if (overlapN) {
		if (env::eCells.terrain_height_ne[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
			env::eCells.terrain_height_se[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y + 1u] ||
			env::eCells.terrain_height_nw[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
			env::eCells.terrain_height_sw[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y + 1u]) {
			height_terrain = m::Lerp(
				(lf32)env::eCells.terrain_height_sw[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y + 1u],
				(lf32)env::eCells.terrain_height_se[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y + 1u],
				offsetx) / TERRAIN_HEIGHT_DIVISION;
			if (height_terrain > (ent->t.altitude + ACTOR_NO_COLLIDE_HEIGHT)) {
				ent->t.position.y = ent->t.csi.c[eCELL_I].y; // + (1 - radius)
				ent->velocity.y = 0.f;
				ent->slideVelocity.y = 0.f;
			}
		}
	}
	// South
	if (overlapS) {
		if (env::eCells.terrain_height_se[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
			env::eCells.terrain_height_ne[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y - 1u] ||
			env::eCells.terrain_height_sw[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
			env::eCells.terrain_height_nw[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y - 1u]) {
			height_terrain = m::Lerp(
				(lf32)env::eCells.terrain_height_nw[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y - 1u],
				(lf32)env::eCells.terrain_height_ne[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y - 1u],
				offsetx) / TERRAIN_HEIGHT_DIVISION;
			if (height_terrain > (ent->t.altitude + ACTOR_NO_COLLIDE_HEIGHT)) {
				ent->t.position.y = ent->t.csi.c[eCELL_I].y; // - (1 - radius)
				ent->velocity.y = 0.f;
				ent->slideVelocity.y = 0.f;
			}
		}
	}
	// East
	if (overlapE) {
		if (env::eCells.terrain_height_ne[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
			env::eCells.terrain_height_nw[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y] ||
			env::eCells.terrain_height_se[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
			env::eCells.terrain_height_sw[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y]) {
			height_terrain = m::Lerp(
				(lf32)env::eCells.terrain_height_nw[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y],
				(lf32)env::eCells.terrain_height_sw[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y],
				offsety) / TERRAIN_HEIGHT_DIVISION;
			if (height_terrain > (ent->t.altitude + ACTOR_NO_COLLIDE_HEIGHT)) {
				ent->t.position.x = ent->t.csi.c[eCELL_I].x; // + (1 - radius)
				ent->velocity.x = 0.f;
				ent->slideVelocity.x = 0.f;
			}
		}
	}
	// West
	if (overlapW) {
		if (env::eCells.terrain_height_nw[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
			env::eCells.terrain_height_ne[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y] ||
			env::eCells.terrain_height_sw[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
			env::eCells.terrain_height_se[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y]) {
			height_terrain = m::Lerp(
				(lf32)env::eCells.terrain_height_ne[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y],
				(lf32)env::eCells.terrain_height_se[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y],
				offsety) / TERRAIN_HEIGHT_DIVISION;
			if (height_terrain > (ent->t.altitude + ACTOR_NO_COLLIDE_HEIGHT)) {
				ent->t.position.x = ent->t.csi.c[eCELL_I].x; // - (1 - radius)
				ent->velocity.x = 0.f;
				ent->slideVelocity.x = 0.f;
			}
		}
	}

	// CORNER COLLISION CHECK

	// TODO: Something up with the corner checks, not sure what

	// North-east
	if (env::eCells.terrain_height_ne[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
		env::eCells.terrain_height_sw[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y + 1u]) {
		if (((lf32)env::eCells.terrain_height_sw[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y + 1u] / TERRAIN_HEIGHT_DIVISION)
		> (ent->t.altitude + ACTOR_NO_COLLIDE_HEIGHT) && overlapN && overlapE) {
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, 0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
	}
	// North-west
	if (env::eCells.terrain_height_nw[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
		env::eCells.terrain_height_se[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y + 1u]) {
		if (((lf32)env::eCells.terrain_height_se[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y + 1u] / TERRAIN_HEIGHT_DIVISION)
			> (ent->t.altitude + ACTOR_NO_COLLIDE_HEIGHT) && overlapN && overlapW) {
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, 0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
	}// South-east
	if (env::eCells.terrain_height_se[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
		env::eCells.terrain_height_nw[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y - 1u]) {
		if (((lf32)env::eCells.terrain_height_nw[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y - 1u] / TERRAIN_HEIGHT_DIVISION)
			> (ent->t.altitude + ACTOR_NO_COLLIDE_HEIGHT) && overlapS && overlapE) {
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, -0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
	}// South-west
	if (env::eCells.terrain_height_sw[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y] !=
		env::eCells.terrain_height_ne[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y - 1u]) {
		if (((lf32)env::eCells.terrain_height_ne[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y - 1u] / TERRAIN_HEIGHT_DIVISION)
			> (ent->t.altitude + ACTOR_NO_COLLIDE_HEIGHT) && overlapS && overlapW) {
			m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, -0.5f);
			if (m::Length(offset) < 0.5f)
				ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
		}
	}
	#endif

}

#undef ACTOR_NO_COLLIDE_HEIGHT

bool RayEntity(LtrID ent, lf32 stand_height)
{
	ECCommon* entity = ENTITY(ent);

	lf32 th;
	#if DEF_GRID
	env::GetHeight(th, entity->t.csi);
	#else
	env::GetNearestSurfaceHeight(th, entity->t.csi, entity->t.altitude);
	#endif
	if (entity->t.altitude + entity->altitude_velocity <= th + stand_height)
		return true;
	return false;
}
void Entity_CheckGrounded(LtrID id, ECCommon* ent)
{
	lf32 th;
	#if DEF_GRID
	env::GetHeight(th, ent->t.csi);
	#else
	env::GetNearestSurfaceHeight(th, ent->t.csi, ent->t.altitude);
	#endif

	if (ent->t.altitude + ent->altitude_velocity >= th + 1.f) {
		ent->activeFlags.unset(ECCommon::eGROUNDED);
		ent->slideVelocity *= 0.f;
		if (GetEntityType(id.Index()) == ENTITY_TYPE_ACTOR)
			((ECActor*)ent)->jump_state = ECActor::eJUMP_JUMP;
	}
	else if (ent->altitude_velocity > 0.f) {
		ent->activeFlags.unset(ECCommon::eGROUNDED);
		ent->slideVelocity *= 0.f;
	}
	else if (!ent->activeFlags.get(ECCommon::eGROUNDED)) {
		if (GetEntityType(id.Index()) == ENTITY_TYPE_ACTOR)
			ent->activeFlags.setto(ECCommon::eGROUNDED, RayEntity(id, ((ECActor*)ent)->aniStandHeight));
		else
			ent->activeFlags.setto(ECCommon::eGROUNDED, RayEntity(id, 0.f));
		if (ent->activeFlags.get(ECCommon::eGROUNDED)) {
			ent->slideVelocity = ent->velocity;
			if (GetEntityType(id.Index()) == ENTITY_TYPE_ACTOR) {
				ActorOnHitGround((ECActor*)ent);
			}
		}
	}
}
void Entity_PhysicsTick(ECCommon* ent, LtrID id, lf32 dt)
{
	// Regenerate csi

	CellSpace cs_last = ent->t.csi;
	core::GetCellSpaceInfo(ent->t.position, ent->t.csi);
	// If the new CS is different, remove us from the last cell and add us to the new one
	/*if (cs_last.c[eCELL_I].x != ent->t.csi.c[eCELL_I].x || cs_last.c[eCELL_I].y != ent->t.csi.c[eCELL_I].y)
	{
	core::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, id);
	core::AddEntityCell(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, id);
	}*/

	Entity_CheckGrounded(id, ent);

	if (!ent->activeFlags.get(ECCommon::eGROUNDED)) {
		// Add gravity
		ent->altitude_velocity -= 0.20f * dt;
		// Velocity reduction (Air drag)
		if (ent->physicsFlags.get(ECCommon::ePHYS_DRAG)) ent->velocity *= 0.99f;

		#if !DEF_GRID
		// ceiling collision
		lf32 ceil_height;
		env::GetNearestCeilingHeight(ceil_height, ent->t.csi, ent->t.altitude);
		if (ceil_height < ent->t.altitude + ent->height) {
			ent->t.altitude = ceil_height - ent->height;
			ent->altitude_velocity = 0.f;
		}
		#endif
	}
	else
	{
		// floor collision
		lf32 ground_height;
		m::Vector2 slope(0.f, 0.f);
		#if DEF_GRID
		env::GetHeight(ground_height, ent->t.csi);
		env::GetSlope(slope.x, slope.y, ent->t.csi);
		#else
		env::EnvTri* triptr = nullptr;
		env::GetNearestSurfaceHeight(ground_height, &triptr, ent->t.csi, ent->t.altitude);
		if (triptr != nullptr) slope = triptr->slope;
		#endif
		slope *= 4.f; // Amplify slope for slide calculation

		// look at how long this is
		#if DEF_GRID
		switch (acv::props[env::eCells.prop[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y]].floorType)
		#else
		// TODO: super temporary
		// will have to get floortype from triangles
		acv::PropRecord::EnvPropFloorMat floor = acv::PropRecord::FLOOR_STANDARD;
		switch (floor)
		#endif
		{
		case acv::PropRecord::FLOOR_QUICKSAND:
		{
			lf32 distBelowSand = 0.f;
			ent->t.altitude -= 0.12f * dt;

			// slow height velocity
			ent->altitude_velocity *= 0.9f;
			ent->slideVelocity *= 0.9f;

			// Multiplier reduces the depth at which you can't move
			distBelowSand = (ent->t.altitude - ground_height) * 1.4f;
			if (-distBelowSand > ent->height) {
				ent->Damage(1000, 0);
			}
			distBelowSand = m::Clamp(distBelowSand, 0.f, 1.f);
			ent->velocity *= distBelowSand;
		}
		break;
		case acv::PropRecord::FLOOR_ICE: // same as normal ground but dont slow slide
		{
			if (GetEntityType(id.Index()) == ENTITY_TYPE_ACTOR) {
				if (ent->t.altitude < ground_height + ((ECActor*)ent)->aniStandHeight * 0.25f)
					ent->t.altitude = ground_height + ((ECActor*)ent)->aniStandHeight * 0.25f;
				else
					ent->t.altitude = m::Lerp(ent->t.altitude, ground_height + ((ECActor*)ent)->aniStandHeight, 6.f * dt);
			}
			else {
				ent->t.altitude = ground_height;
				ent->velocity *= 0.f; // Remove slide on non actors
			}

			m::Vector2 surfMod(1.f, 1.f);

			// a reasonable implementation would look like this
			ent->slideVelocity += ent->velocity * 0.1f;
			// Slide speed cap
			lf32 slide_len = m::Length(ent->slideVelocity);
			if (slide_len > 3.f * dt)
				ent->slideVelocity = m::Normalize(ent->slideVelocity) * 3.f * dt;

			//ent->slideVelocity += ent->velocity * 1.1f;

			// Don't slide uphill
			if (ent->slideVelocity.x > 0.f && slope.x > 0.f) {
				surfMod.x = 0.5f;
				slope.x = 0.f;
			}
			else if (ent->slideVelocity.x < 0.f && slope.x < 0.f) {
				surfMod.x = 0.5f;
				slope.x = 0.f;
			}
			if (ent->slideVelocity.y > 0.f && slope.y > 0.f) {
				surfMod.y = 0.5f;
				slope.y = 0.f;
			}
			else if (ent->slideVelocity.y < 0.f && slope.y < 0.f) {
				surfMod.y = 0.5f;
				slope.y = 0.f;
			}

			// Slide reduction per second multiplied by frame length
			lf32 slide_reduce = 0.005f * dt;

			//if (GetEntityType(id) == ENTITY_TYPE_ACTOR)
			//	if (((ECActor*)ent)->aniCrouch)
			//		slide_reduce *= 0.125f; // Slide longer if crouching

			// Linear slide reduction (with slope adjustment)
			m::Vector2 slideMag = (lf32)m::Length(ent->slideVelocity);
			slideMag.y = slideMag.x;
			if (slideMag.x > slide_reduce / (abs(slope.x) + surfMod.x)) {
				slideMag.x -= slide_reduce / (abs(slope.x) + surfMod.x);
			}
			else slideMag.x = 0.f;
			if (slideMag.y > slide_reduce / (abs(slope.y) + surfMod.y)) {
				slideMag.y -= slide_reduce / (abs(slope.y) + surfMod.y);
			}
			else slideMag.y = 0.f;
			ent->slideVelocity = m::Normalize(ent->slideVelocity) * slideMag;

			// No height velocity when on the ground!
			ent->altitude_velocity = 0.f;
		}
		break;
		case acv::PropRecord::FLOOR_LAVA:
			if (ent->activeFlags.get(ECCommon::eALIVE)) {
				ent->Damage(1000u, 0.f);
			}
			// no break, use default physics
			goto defaul;
		case acv::PropRecord::FLOOR_ACID:
			if (ent->activeFlags.get(ECCommon::eALIVE)) {
				ent->Damage(1000u, 0.f);
				if (IDCOMPARE(id, core::players[0])) {
					char string[64] = "The acid desintegrated your save file";
					remove("save/save.bin");
					core::GUISetMessag(0, string);
				}
				else if (IDCOMPARE(id, core::players[1])) {
					char string[64] = "The acid desintegrated your save file";
					remove("save/save.bin");
					core::GUISetMessag(1, string);
				}
			}
			// no break, use default physics
		defaul:
		default:
		{
			if (GetEntityType(id.Index()) == ENTITY_TYPE_ACTOR) {
				ent->t.altitude = ground_height + ((ECActor*)ent)->aniStandHeight;
				// this will work, but we need to address the sprint-jump problem
				//if (ent->t.height < ground_height + ((ECActor*)ent)->aniStandHeight * 0.4f)
				//	ent->t.height = ground_height + ((ECActor*)ent)->aniStandHeight * 0.4f;
				//else
				//	ent->t.height = m::Lerp(ent->t.height, ground_height + ((ECActor*)ent)->aniStandHeight, 6.f * dt);
			}
			else {
				ent->t.altitude = ground_height;
				ent->velocity *= 0.f; // Remove slide on non actors
			}

			m::Vector2 surfMod(1.f, 1.f);

			// Don't slide uphill
			if (ent->slideVelocity.x > 0.f && slope.x > 0.f) {
				surfMod.x = 0.5f;
				slope.x = 0.f;
			}
			else if (ent->slideVelocity.x < 0.f && slope.x < 0.f) {
				surfMod.x = 0.5f;
				slope.x = 0.f;
			}
			if (ent->slideVelocity.y > 0.f && slope.y > 0.f) {
				surfMod.y = 0.5f;
				slope.y = 0.f;
			}
			else if (ent->slideVelocity.y < 0.f && slope.y < 0.f) {
				surfMod.y = 0.5f;
				slope.y = 0.f;
			}

			lf32 slide_reduce = 0.18f * dt; // Slide reduction per second multiplied by frame length

			if (GetEntityType(id.Index()) == ENTITY_TYPE_ACTOR)
				if (((ECActor*)ent)->aniCrouch)
					slide_reduce *= 0.25f; // Slide longer if crouching

			// Linear slide reduction (with slope adjustment)
			m::Vector2 slideMag = (lf32)m::Length(ent->slideVelocity);
			slideMag.y = slideMag.x;
			if (slideMag.x > slide_reduce / (abs(slope.x) + surfMod.x)) {
				slideMag.x -= slide_reduce / (abs(slope.x) + surfMod.x);
			}
			else slideMag.x = 0.f;
			if (slideMag.y > slide_reduce / (abs(slope.y) + surfMod.y)) {
				slideMag.y -= slide_reduce / (abs(slope.y) + surfMod.y);
			}
			else slideMag.y = 0.f;
			ent->slideVelocity = m::Normalize(ent->slideVelocity) * slideMag;

			// No height velocity when on the ground!
			ent->altitude_velocity = 0.f;
		}
		break;
		}

		ent->t.position += ent->slideVelocity;
	}

	#if DEF_CCD // Swap collision/velocity apply order
	// Perform collision
	Entity_Collision(id, ent, ent->t.csi);
	// Apply velocity
	ent->t.position += ent->velocity;
	ent->t.altitude += ent->altitude_velocity;
	#else
	// Apply velocity
	ent->t.position += ent->velocity;
	ent->t.altitude += ent->altitude_velocity;
	// Perform collision
	Entity_Collision(id, ent, ent->t.csi);
	#endif

	// duplicate of start of function
	core::GetCellSpaceInfo(ent->t.position, ent->t.csi);
	// If the new CS is different, remove us from the last cell and add us to the new one
	if (cs_last.c[eCELL_I].x != ent->t.csi.c[eCELL_I].x || cs_last.c[eCELL_I].y != ent->t.csi.c[eCELL_I].y)
	{
		core::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, id);
		core::AddEntityCell(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, id);
	}
}

char* EntityName(void* ent) {
	return (char*)((ECCommon*)ent)->name;
};
