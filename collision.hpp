#pragma once

#pragma once

#include "maths.hpp"

namespace collision
{
	struct hit_info {
		bool hit = false;
		fw::Vector2 surface = fw::Vector2(0.f, 0.f);
		fw::Vector2 depenetrate = fw::Vector2(0.f, 0.f);
		fw::Vector2 inheritedVelocity = fw::Vector2(0.f,0.f);
	};
}