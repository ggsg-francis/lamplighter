#include "weather.h"
#include "glm\glm.hpp"
#include "maths.hpp"
//#include "global.h"
#include <iostream>

namespace weather
{
	//templates
	glm::vec3 col_clear_sun = glm::vec3(1.1f, 1.05f, 0.95f);
	//glm::vec3 col_clear_amb = glm::vec3(0.34f, 0.5f, 0.8f);
	glm::vec3 col_clear_amb = glm::vec3(0.64f, 0.9f, 0.96f);
	glm::vec3 col_clear_fog = glm::vec3(0.7f, 0.73f, 0.77f);

	glm::vec3 col_foggy_sun = glm::vec3(0.56f, 0.58f, 0.6f);
	glm::vec3 col_foggy_amb = glm::vec3(0.2f, 0.3f, 0.35f);
	glm::vec3 col_foggy_fog = glm::vec3(0.35f, 0.37f, 0.4f);

	//current
	glm::vec3 col_sun = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 col_amb = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 col_fog = glm::vec3(0.f, 0.f, 0.f);

	glm::vec3 col_sun_from = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 col_amb_from = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 col_fog_from = glm::vec3(0.f, 0.f, 0.f);

	glm::vec3 col_sun_dest = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 col_amb_dest = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 col_fog_dest = glm::vec3(1.f, 1.f, 1.f);

	float fog_level = 0.f;
	float fog_level_from = 0.f;
	float fog_level_dest = 0.f;

	//start at 1 to trigger a weather transition right at the start
	float recalc_ticker = 1.f;

	float stat_tod;
	float stat_overcast_level;

//#define TIMER_TIME 120.f
#define TIMER_TIME 60.f
//#define TIMER_TIME 2.f

	void Tick(btf32 dt)
	{
		/*
		recalc_ticker += dt / TIMER_TIME;
		if (recalc_ticker > 1.f)
		{
			//recalc_ticker += 60.f;
			recalc_ticker -= 1.f;

			//setup the new transition values
			col_sun_from = col_sun_dest;
			col_amb_from = col_amb_dest;
			col_fog_from = col_fog_dest;
			fog_level_from = fog_level_dest;

			float lo_bri = 0.1f;
			float hi_bri = 1.f;
			float b = lo_bri + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi_bri - lo_bri)));
			float lo_hue = -0.05f;
			float hi_hue = 0.05f;
			//randomizer number
			float r1 = lo_hue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi_hue - lo_hue)));
			float r2 = lo_hue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi_hue - lo_hue)));
			float r3 = lo_hue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi_hue - lo_hue)));

			stat_overcast_level = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			//stat_overcast_level = 1.f; //temp foggy override
			std::cout << "Recalculated weather, overcast level " << stat_overcast_level << std::endl;

			if (stat_overcast_level > 0.5f)
			{
				//interpolate light colour based on sunny/foggy value
				col_amb_dest.x = m::Lerp(col_clear_amb.x, (col_foggy_amb.x + r1) * b, stat_overcast_level);
				col_amb_dest.y = m::Lerp(col_clear_amb.y, (col_foggy_amb.y + r2) * b, stat_overcast_level);
				col_amb_dest.z = m::Lerp(col_clear_amb.z, (col_foggy_amb.z + r3) * b, stat_overcast_level);

				col_sun_dest.x = m::Lerp(col_clear_sun.x, (col_foggy_sun.x + (r1 * 0.25f)) * b, stat_overcast_level);
				col_sun_dest.y = m::Lerp(col_clear_sun.y, (col_foggy_sun.y + (r2 * 0.25f)) * b, stat_overcast_level);
				col_sun_dest.z = m::Lerp(col_clear_sun.z, (col_foggy_sun.z + (r3 * 0.25f)) * b, stat_overcast_level);
			}
			else
			{
				col_amb_dest.x = m::Lerp(col_clear_amb.x, col_foggy_amb.x, stat_overcast_level);
				col_amb_dest.y = m::Lerp(col_clear_amb.y, col_foggy_amb.y, stat_overcast_level);
				col_amb_dest.z = m::Lerp(col_clear_amb.z, col_foggy_amb.z, stat_overcast_level);

				col_sun_dest.x = m::Lerp(col_clear_sun.x, col_foggy_sun.x, stat_overcast_level);
				col_sun_dest.y = m::Lerp(col_clear_sun.y, col_foggy_sun.y, stat_overcast_level);
				col_sun_dest.z = m::Lerp(col_clear_sun.z, col_foggy_sun.z, stat_overcast_level);
			}
			col_fog_dest.x = m::Lerp(col_clear_fog.x, (col_foggy_fog.x + (r1 * 0.5f)) * b, stat_overcast_level);
			col_fog_dest.y = m::Lerp(col_clear_fog.y, (col_foggy_fog.y + (r2 * 0.5f)) * b, stat_overcast_level);
			col_fog_dest.z = m::Lerp(col_clear_fog.z, (col_foggy_fog.z + (r3 * 0.5f)) * b, stat_overcast_level);

			if (stat_overcast_level > 0.8f)
				fog_level_dest = 1.f;
			else
				fog_level_dest = 0.f;
		}

		float transition_point = recalc_ticker;

		//add col_sun_from, then it will work

		col_sun.x = m::Lerp(col_sun_from.x, col_sun_dest.x, transition_point);
		col_sun.y = m::Lerp(col_sun_from.y, col_sun_dest.y, transition_point);
		col_sun.z = m::Lerp(col_sun_from.z, col_sun_dest.z, transition_point);

		col_amb.x = m::Lerp(col_amb_from.x, col_amb_dest.x, transition_point);
		col_amb.y = m::Lerp(col_amb_from.y, col_amb_dest.y, transition_point);
		col_amb.z = m::Lerp(col_amb_from.z, col_amb_dest.z, transition_point);

		col_fog.x = m::Lerp(col_fog_from.x, col_fog_dest.x, transition_point);
		col_fog.y = m::Lerp(col_fog_from.y, col_fog_dest.y, transition_point);
		col_fog.z = m::Lerp(col_fog_from.z, col_fog_dest.z, transition_point);

		fog_level = m::Lerp(fog_level_from, fog_level_dest, transition_point);
		*/
	}

	void* SunColour()
	{
		return (void*)&col_sun;
	}
	void* AmbientColour()
	{
		return (void*)&col_amb;
	}
	void* FogColour()
	{
		return (void*)&col_fog;
	}
	void* FogDensity()
	{
		return (void*)&fog_level;
	}
}