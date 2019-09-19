#include "factions.h"

namespace fac
{
	struct fac_storage
	{
		facalleg allegiances[num_factions]{neutral};
	};
	fac_storage facs[num_factions];

	facalleg GetAllegiance(faction fac_a, faction fac_b)
	{
		return facs[fac_a].allegiances[fac_b];
	}

	void SetAllegiance(faction fac_a, faction fac_b, facalleg lvl)
	{
		facs[fac_a].allegiances[fac_b] = lvl;
	}
}