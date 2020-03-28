# bt3d

Library install:
The solution expects libraries to exist in ../libraries/ relatively.
Below is the expected folder layout:

libraries/
	include/
		zlib/
			... (zlib headers)
		SDL2/
			... (SDL headers)
	lib/
		SDL2.lib
		SDL2_net.lib
		zlib.lib
		zlibstatic.lib
bt3d/ ('git1' repository folder)
	0BUILD_RELEASE/
		SDL2.dll
		zlib1.dll
	1BUILD_RELEASE_NMP/
		SDL2.dll
		SDL2_net.dll
		zlib1.dll
