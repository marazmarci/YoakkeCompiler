#pragma once

#include "../common.h"

namespace yk {
	namespace ext {
		template <typename K, typename T>
		T* get_value(ymap<K, T>& map, K const& val) {
			auto it = map.find(val);
			if (it != map.end()) {
				return it->second;
			}
			else {
				return nullptr;
			}
		}
	}
}