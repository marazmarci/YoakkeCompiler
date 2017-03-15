#include <Windows.h>
#include <iostream>
#include "src\utility\opt_map.h"

int main(void) {
	yk::opt_map<std::string, int> map;
	map.insert(std::make_pair("hello", 1));
	if (auto val = map.at("hello")) {
		std::cout << "found!" << std::endl;
	}

	system("PAUSE");
	return 0;
}
