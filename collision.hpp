#pragma once

#pragma once

#include "maths.hpp"

namespace collision
{
	struct hit_info {
		bool hit = false;
		m::Vector2 surface = m::Vector2(0.f, 0.f);
		m::Vector2 depenetrate = m::Vector2(0.f, 0.f);
		m::Vector2 inheritedVelocity = m::Vector2(0.f,0.f);
	};
}