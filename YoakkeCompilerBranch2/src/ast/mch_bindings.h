#pragma once

#include <mach7/match.hpp>
#include <mach7/patterns/constructor.hpp>

#include "expr.h"
#include "pattern.h"
#include "stmt.h"
#include "type_desc.h"

namespace mch {
	using namespace yk;

	// Statements
	template <> struct bindings<expr_stmt>		{ Members(expr_stmt::Expression, expr_stmt::Semicol); };

	// Expressions
	template <> struct bindings<ident_expr>		{ Members(ident_expr::Identifier); };
	template <> struct bindings<unit_expr>		{ };
	template <> struct bindings<int_lit_expr>	{ Members(int_lit_expr::Value); };
	template <> struct bindings<real_lit_expr>	{ Members(real_lit_expr::Value); };
	template <> struct bindings<preury_expr>	{ Members(preury_expr::Operator, preury_expr::Sub); };
	template <> struct bindings<postury_expr>	{ Members(postury_expr::Operator, postury_expr::Sub); };
	template <> struct bindings<binop_expr>		{ Members(binop_expr::Operator, binop_expr::LHS, binop_expr::RHS); };
	template <> struct bindings<asgn_expr>		{ Members(asgn_expr::LHS, asgn_expr::RHS); };
	template <> struct bindings<const_asgn_expr>{ Members(const_asgn_expr::LHS, const_asgn_expr::RHS); };
	template <> struct bindings<list_expr>		{ Members(list_expr::Elements); };
	template <> struct bindings<call_expr>		{ Members(call_expr::Function, call_expr::Args); };
	template <> struct bindings<block_expr>		{ Members(block_expr::Statements); };
	template <> struct bindings<fnproto_expr>	{ Members(fnproto_expr::Parameters, fnproto_expr::ReturnType); };
	template <> struct bindings<fn_expr>		{ Members(fn_expr::Prototype, fn_expr::Body); };
	template <> struct bindings<let_expr>		{ Members(let_expr::Pattern, let_expr::Type, let_expr::Value); };

	// Patterns
	template <> struct bindings<ident_pattern>	{ Members(ident_pattern::Identifier); };
	template <> struct bindings<bin_pattern>	{ Members(bin_pattern::Operator, bin_pattern::LHS, bin_pattern::RHS); };
	template <> struct bindings<list_pattern>	{ Members(list_pattern::Elements); };

	// Type descriptors
	template <> struct bindings<ident_type_desc>{ Members(ident_type_desc::Identifier); };
	template <> struct bindings<bin_type_desc>	{ Members(bin_type_desc::Operator, bin_type_desc::LHS, bin_type_desc::RHS); };
	template <> struct bindings<list_type_desc> { Members(list_type_desc::Elements); };
}
