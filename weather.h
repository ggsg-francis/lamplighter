#ifndef WEATHER_H
#define WEATHER_H

#include "global.h"

namespace weather
{
	void Tick(btf32 DELTA);

	void* SunColour();
	void* AmbientColour();
	void* FogColour();
	void* FogDensity();
}

#endif
