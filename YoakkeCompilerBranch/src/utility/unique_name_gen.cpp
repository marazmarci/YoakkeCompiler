#include <cctype>
#include "unique_name_gen.h"

namespace yk {
	ystr unique_name_gen::get(ystr const& ids2) {
		ystr ids = (ids2 == "" ? "0" : ids2);
		// Cut off number
		ysize num_part = 0;
		for (int i = ids.size() - 1; i >= 0; i--) {
			if (std::isdigit(ids[i])) {
				num_part++;
			}
			else {
				break;
			}
		}
		ids = ids.substr(0, ids.size() - num_part);

		auto it = Names.find(ids);
		if (it == Names.end()) {
			Names.insert(std::make_pair(ids, 1));
			return ids;
		}
		else {
			return ids + std::to_string(++it->second);
		}
	}
}