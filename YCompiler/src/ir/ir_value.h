#pragma once

#include "../common.h"

struct ir_type;
struct ir_sized_type;
struct ir_integer_type;

struct ir_value {
public:
	ir_type* Type;

protected:
	ir_value(ir_type* t);

public:
	virtual ~ir_value();
};

struct ir_named_value : public ir_value {
public:
	ystr Name;

protected:
	ir_named_value(ir_sized_type* ty, ystr const& name);

public:
	virtual ~ir_named_value();
};

struct ir_integer_value : public ir_value {
public:
	long long int Value;

private:
	ir_integer_value(ir_integer_type* ty, long long int val);

public:
	static ir_integer_value* create(ysize bits, bool sign, long long int val);
	static ir_integer_value* create_32(bool sign, long long int val);
	static ir_integer_value* create_i32(long long int val);
	static ir_integer_value* create_i8(long long int val);
};

struct ir_fp_value : public ir_value {
public:
	long double Value;

private:
	ir_fp_value(ir_fp_type* ty, long double val);

public:
	static ir_fp_value* create(ysize bits, long double val);
	static ir_fp_value* create_32(long long int val);
};
