#pragma once

#include <cassert>
#include "type.h"
#include "symbol.h"
#include "../common.h"

struct type;
struct symbol;

struct scope {
	scope*				Parent;
	ymap<ystr, type*>	Types;
	ymap<ystr, symbol*>	Symbols;
	bool				ReturnDest;
	type*				ReturnType;
	yopt<interval>		ReturnPos;
	bool				Free;

	scope(bool retDest, bool free);
	~scope();

	yopt<type*> ref_type(ystr const& name);
	yopt<symbol*> ref_sym(ystr const& name);

	yopt<type*> local_ref_type(ystr const& name);
	yopt<symbol*> local_ref_sym(ystr const& name);

	void decl_type(ystr const& name, type* ty);
	void decl_sym(symbol* sym);

	yopt<symbol*> remove_symbol(ystr const& name);
	yopt<type*> remove_type(ystr const& name);

	bool shadow_symbol(ystr const& name);
	bool shadow_type(ystr const& name);

private:
	template <typename T>
	yopt<T> local_ref(ymap<ystr, T>& map, ystr const& name) {
		auto it = map.find(name);
		if (it == map.end()) {
			return {};
		}
		return it->second;
	}

	template <typename T>
	void decl(ymap<ystr, T>& map, ystr const& name, T t) {
		assert(map.find(name) == map.end() && "Cannot override symbol/type!");

		map.insert({ name, t });
	}

	template <typename T>
	yopt<T> remove(ymap<ystr, T>& map, ystr const& name) {
		auto it = map.find(name);
		if (it == map.end()) {
			return {};
		}
		T res = it->second;
		map.erase(it);
		return res;
	}

	template <typename T>
	bool shadow(ymap<ystr, T>& map, ystr const& name) {
		static const char shadowChar = '$';

		auto it = map.find(name);
		if (it != map.end()) {
			ystr newName = shadowChar + name;
			T t = it->second;
			map.erase(it);
			shadow(map, newName);
			map.insert({ newName, t });
			return true;
		}
		return false;
	}
};
