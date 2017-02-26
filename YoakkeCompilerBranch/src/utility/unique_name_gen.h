#pragma once

#include "../common.h"

namespace yk {
	class unique_name_gen {
	public:
		ymap<ystr, ysize> Names;
		yset<ystr> NameSet;

	public:
		ystr get(ystr const& ids2);
	};
}
