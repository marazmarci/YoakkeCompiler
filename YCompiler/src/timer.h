#pragma once

#include <chrono>
#include "common.h"

struct timer {
public:
	using time_point = std::chrono::high_resolution_clock::time_point;

public:
	void start();
	ysize end();

private:
	time_point m_Start;
	time_point m_End;
};
