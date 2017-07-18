#include <iostream>
#include "io/file_hnd.h"
#include "io/fmt_code.h"

int main(void) {
	file_hnd file("C:/TMP/YoakkeTest/tokenizer.txt");
	if (!file.good()) {
		std::cout << "Could not open file: " << file.path() << '!' << std::endl;
	}

	fmt_code::print(file, interval(point(4, 5), 3));

	std::cin.get();
	return 0;
}
