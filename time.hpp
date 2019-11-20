#pragma once

namespace Time
{
	extern btf64 time;
	extern btf64 last;
	extern btf64 lastTick;
	extern btf64 deltaTime;
	extern btf64 deltaTick;

	// Per-frame time delta calculation
	inline void Update(btf64 _time)
	{
		time = _time;
		deltaTime = time - last;
		last = time;
	}

	inline void Step()
	{
		Time::deltaTick = Time::time - Time::lastTick;
		Time::lastTick = Time::time;
	}
}