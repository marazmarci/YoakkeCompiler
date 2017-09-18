#pragma once

#include "../common.h"

enum class ir_type_t {
	Void,
	Integer,
	FloatingPoint,
};

struct ir_type {
public:
	ir_type_t Ty;

protected:
	ir_type(ir_type_t t);

public:
	virtual ~ir_type();
};

struct ir_void_type : public ir_type {
	ir_void_type();
	virtual ~ir_void_type();
};

struct ir_sized_type : public ir_type {
public:
	ysize Bits;

protected:
	ir_sized_type(ir_type_t t, ysize bits);

public:
	virtual ~ir_sized_type();
};

struct ir_integer_type : public ir_sized_type {
	bool Signed;

	ir_integer_type(ysize bits, bool sig);
	virtual ~ir_integer_type();
};

struct ir_fp_type : public ir_sized_type {
	ir_fp_type(ysize bits);
	virtual ~ir_fp_type();
};
