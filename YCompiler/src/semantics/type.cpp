#include "type.h"

type::type(type_t tt)
	: Ty(tt) {
}

type::~type() { }

/*****************************************************************************/

ysize type_var::s_IDcounter = 0;

type_var::type_var()
	: type(type_t::Variable), ID(s_IDcounter++), Instance(nullptr) {
}

type_var::~type_var() { }

/*****************************************************************************/

type_cons::type_cons(ystr const& name, yvec<type*> const& params)
	: type(type_t::Constructor), Name(name), Params(params) {
}

type_cons::~type_cons() { }

void type_cons::add(type* t) {
	Params.push_back(t);
}

type_cons* type_cons::fn(type* params, type* ret) {
	return new type_cons(type_prefixes::Function, { params, ret });
}

type_cons* type_cons::generic_fn() {
	return new type_cons(type_prefixes::Function, { new type_var(), new type_var() });
}

/*****************************************************************************/
