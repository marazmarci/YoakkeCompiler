#include <iostream>
#include "io/file_hnd.h"

int main(void) {
	file_hnd file("C:/TMP/YoakkeTest/tokenizer.txt");
	if (!file.good()) {
		std::cout << "Could not open file: " << file.path() << '!' << std::endl;
	}

	std::cin.get();
	return 0;
}
