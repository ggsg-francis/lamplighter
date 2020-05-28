#ifndef FACTIONS_H
#define FACTIONS_H

#include "global.h"

namespace fac
{
	// What are the factions
	enum faction : btui8
	{
		none,
		player,
		undead,
		playerhunter,
		num_factions, // Automatic number of factions (for arrays)
	};

	// How does a faction treat another faction
	enum facalleg : btui8
	{
		neutral, // not yet used
		liked, // not yet used
		allied,
		dislike, // not yet used
		enemy,
	};

	facalleg GetAllegiance(faction FACTION_THIS, faction FACTION_OTHER);
	void SetAllegiance(faction FACTION_THIS, faction FACTION_OTHER, facalleg LEVEL);
}

#endif
