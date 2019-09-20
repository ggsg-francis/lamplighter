#pragma once

namespace aud
{
	#ifdef DEF_USE_CS
	void Init(void* handle);
	#else
	void Init();
	#endif
	void PlaySnd();
	void End();
}