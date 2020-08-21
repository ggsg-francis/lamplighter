#include "weather.h"
#include "maths.hpp"
//#include "global.h"
#include <iostream>

#include "cfg.h"

namespace weather
{
	Weather w;

#define TIMER_TIME 120.f
//#define TIMER_TIME 20.f

	void Tick(btf32 dt)
	{
		w.recalc_ticker += dt / TIMER_TIME;
		if (w.recalc_ticker > 1.f)
		{
			//recalc_ticker += 60.f;
			w.recalc_ticker -= 1.f;

			//setup the new transition values
			w.col_sun_from = w.col_sun_dest;
			w.col_amb_from = w.col_amb_dest;
			w.col_fog_from = w.col_fog_dest;
			w.fog_level_from = w.fog_level_dest;

			float lo_bri = 0.1f;
			float hi_bri = 1.f;
			float b = lo_bri + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi_bri - lo_bri)));
			float lo_hue = -0.05f;
			float hi_hue = 0.05f;
			//randomizer number
			float r1 = lo_hue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi_hue - lo_hue)));
			float r2 = lo_hue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi_hue - lo_hue)));
			float r3 = lo_hue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi_hue - lo_hue)));

			w.stat_overcast_level = m::Random(0.f, 1.f);
			if (cfg::bEditMode) w.stat_overcast_level = 0.f; // Overcast override
			std::cout << "Recalculated weather, overcast level " << w.stat_overcast_level << std::endl;

			if (w.stat_overcast_level > 0.5f)
			{
				//interpolate light colour based on sunny/foggy value
				w.col_amb_dest.x = m::Lerp(w.col_clear_amb.x, (w.col_foggy_amb.x + r1) * b, w.stat_overcast_level);
				w.col_amb_dest.y = m::Lerp(w.col_clear_amb.y, (w.col_foggy_amb.y + r2) * b, w.stat_overcast_level);
				w.col_amb_dest.z = m::Lerp(w.col_clear_amb.z, (w.col_foggy_amb.z + r3) * b, w.stat_overcast_level);
				w.col_sun_dest.x = m::Lerp(w.col_clear_sun.x, (w.col_foggy_sun.x + (r1 * 0.25f)) * b, w.stat_overcast_level);
				w.col_sun_dest.y = m::Lerp(w.col_clear_sun.y, (w.col_foggy_sun.y + (r2 * 0.25f)) * b, w.stat_overcast_level);
				w.col_sun_dest.z = m::Lerp(w.col_clear_sun.z, (w.col_foggy_sun.z + (r3 * 0.25f)) * b, w.stat_overcast_level);
			}
			else
			{
				w.col_amb_dest.x = m::Lerp(w.col_clear_amb.x, w.col_foggy_amb.x, w.stat_overcast_level);
				w.col_amb_dest.y = m::Lerp(w.col_clear_amb.y, w.col_foggy_amb.y, w.stat_overcast_level);
				w.col_amb_dest.z = m::Lerp(w.col_clear_amb.z, w.col_foggy_amb.z, w.stat_overcast_level);
				w.col_sun_dest.x = m::Lerp(w.col_clear_sun.x, w.col_foggy_sun.x, w.stat_overcast_level);
				w.col_sun_dest.y = m::Lerp(w.col_clear_sun.y, w.col_foggy_sun.y, w.stat_overcast_level);
				w.col_sun_dest.z = m::Lerp(w.col_clear_sun.z, w.col_foggy_sun.z, w.stat_overcast_level);
			}
			w.col_fog_dest.x = m::Lerp(w.col_clear_fog.x, (w.col_foggy_fog.x + (r1 * 0.5f)) * b, w.stat_overcast_level);
			w.col_fog_dest.y = m::Lerp(w.col_clear_fog.y, (w.col_foggy_fog.y + (r2 * 0.5f)) * b, w.stat_overcast_level);
			w.col_fog_dest.z = m::Lerp(w.col_clear_fog.z, (w.col_foggy_fog.z + (r3 * 0.5f)) * b, w.stat_overcast_level);

			if (w.stat_overcast_level > 0.8f)
				w.fog_level_dest = m::Random(0.01f, 0.15f);
			else
				w.fog_level_dest = 0.015f;
			// temp fog override
			//fog_level_dest = m::Random(0.01f, 0.15f);
		}

		float transition_point = w.recalc_ticker;

		//add col_sun_from, then it will work

		w.col_sun.x = m::Lerp(w.col_sun_from.x, w.col_sun_dest.x, transition_point);
		w.col_sun.y = m::Lerp(w.col_sun_from.y, w.col_sun_dest.y, transition_point);
		w.col_sun.z = m::Lerp(w.col_sun_from.z, w.col_sun_dest.z, transition_point);
		w.col_amb.x = m::Lerp(w.col_amb_from.x, w.col_amb_dest.x, transition_point);
		w.col_amb.y = m::Lerp(w.col_amb_from.y, w.col_amb_dest.y, transition_point);
		w.col_amb.z = m::Lerp(w.col_amb_from.z, w.col_amb_dest.z, transition_point);
		w.col_fog.x = m::Lerp(w.col_fog_from.x, w.col_fog_dest.x, transition_point);
		w.col_fog.y = m::Lerp(w.col_fog_from.y, w.col_fog_dest.y, transition_point);
		w.col_fog.z = m::Lerp(w.col_fog_from.z, w.col_fog_dest.z, transition_point);

		w.fog_level = m::Lerp(w.fog_level_from, w.fog_level_dest, transition_point);
	}

	void* SunColour()
	{
		return (void*)&w.col_sun;
	}
	void* AmbientColour()
	{
		return (void*)&w.col_amb;
	}
	void* FogColour()
	{
		return (void*)&w.col_fog;
	}
	void* FogDensity()
	{
		return (void*)&w.fog_level;
	}
}