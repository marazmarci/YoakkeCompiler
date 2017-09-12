#include "timer.h"

void timer::start() {
	m_Start = std::chrono::high_resolution_clock::now();
}

ysize timer::end() {
	m_End = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(m_End - m_Start).count();
}
