#pragma once

namespace weather
{
	void Tick(float DELTA);

	void* SunColour();
	void* AmbientColour();
	void* FogColour();
	void* FogDensity();
}