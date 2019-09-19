#pragma once

namespace weather
{
	void Tick(btf32 DELTA);

	void* SunColour();
	void* AmbientColour();
	void* FogColour();
	void* FogDensity();
}