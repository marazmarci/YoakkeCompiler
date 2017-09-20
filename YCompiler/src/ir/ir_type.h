#pragma once

#include "../common.h"

enum class ir_type_t {
	Void,
	Integer,
	FloatingPoint,
};

struct ir_integer_type;
struct ir_fp_type;

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
public:
	static ir_integer_type* I32;
	static ir_integer_type* U32;
	static ir_integer_type* I8;
	static ir_integer_type* U8;

public:
	bool Signed;

protected:
	ir_integer_type(ysize bits, bool sig);

public:
	virtual ~ir_integer_type();

public:
	static ir_integer_type* get(ysize bits, bool sig);
};

struct ir_fp_type : public ir_sized_type {
public:
	static ir_fp_type* F32;

protected:
	ir_fp_type(ysize bits);

public:
	virtual ~ir_fp_type();

public:
	static ir_fp_type* get(ysize bits);
};
