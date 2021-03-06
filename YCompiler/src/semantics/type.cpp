#include "type.h"

type::type(type_t tt)
	: Ty(tt), DefPos({}) {
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
	assert(params->Ty == type_t::Constructor);
	assert(((type_cons*)params)->Name == type_prefixes::ParamList);
	return new type_cons(type_prefixes::Function, { params, ret });
}

type_cons* type_cons::generic_fn() {
	return new type_cons(type_prefixes::Function, { new type_var(), new type_var() });
}

type_cons* type_cons::params(yvec<type*> const& params) {
	return new type_cons(type_prefixes::ParamList, params);
}

type_cons* type_cons::generic_params(ysize sz) {
	yvec<type*> ts;
	for (ysize i = 0; i < sz; i++) {
		ts.push_back(new type_var());
	}
	return new type_cons(type_prefixes::ParamList, ts);
}

type_cons* type_cons::generic_tuple(ysize sz) {
	yvec<type*> ts;
	for (ysize i = 0; i < sz; i++) {
		ts.push_back(new type_var());
	}
	return new type_cons(type_prefixes::Tuple, ts);
}

/*****************************************************************************/
