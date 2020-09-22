#ifndef WEATHER_H
#define WEATHER_H

#include "glm/glm.hpp"

#include "global.h"

namespace weather
{
	struct Weather
	{
		//templates
		//glm::vec3 col_clear_sun = glm::vec3(1.6f, 1.55f, 1.45f);
		glm::vec3 col_clear_sun = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 col_clear_amb = glm::vec3(0.64f, 0.9f, 0.96f);
		glm::vec3 col_clear_fog = glm::vec3(0.7f, 0.73f, 0.77f);

		glm::vec3 col_foggy_sun = glm::vec3(0.56f * 0.5f, 0.58f * 0.5f, 0.6f * 0.5f);
		glm::vec3 col_foggy_amb = glm::vec3(0.2f, 0.25f, 0.275f);
		glm::vec3 col_foggy_fog = glm::vec3(0.35f * 0.75f, 0.37f * 0.75f, 0.4f * 0.75f);

		//current
		glm::vec3 col_sun = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 col_amb = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 col_fog = glm::vec3(1.f, 1.f, 1.f);

		glm::vec3 col_sun_from = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 col_amb_from = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 col_fog_from = glm::vec3(1.f, 1.f, 1.f);

		glm::vec3 col_sun_dest = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 col_amb_dest = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 col_fog_dest = glm::vec3(1.f, 1.f, 1.f);

		float fog_level = 0.f;
		float fog_level_from = 0.f;
		float fog_level_dest = 0.f;

		//start at 1 to trigger a weather transition right at the start
		float recalc_ticker = 1.f;

		float stat_tod = 0.f;
		float stat_overcast_level = 0.f;
	};

	extern Weather w;

	void Tick(btf32 DELTA);

	void* SunColour();
	void* AmbientColour();
	void* FogColour();
	void* FogDensity();
}

#endif
