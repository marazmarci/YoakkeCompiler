#pragma once

#include "../Common.h"

namespace yk
{
	namespace ext
	{
		template <typename K, typename V>
		V* GetValue(ymap<K, V*>& map, K const& k)
		{
			auto it = map.find(k);
			if (it == map.end())
				return nullptr;
			return it->second;
		}
	}
}