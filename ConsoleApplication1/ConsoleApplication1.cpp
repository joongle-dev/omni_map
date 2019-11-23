#include <iostream>
#include "omni_map.h"

int main()
{
	util::omni_map<int, std::string> test_map;
	test_map.insert(3, "three");
	test_map.insert(2, "two");
	test_map.insert(7, "seven");
	test_map.insert(1, "one");
	test_map.insert(6, "six");
	test_map.insert(4, "four");
	test_map.insert(5, "five");
	auto it0 = test_map.find<0>(4);
	std::cout << std::get<0>(*it0) << " " << std::get<1>(*it0) << std::endl;
	auto it1 = test_map.find<1>("seven");
	std::cout << std::get<0>(*it1) << " " << std::get<1>(*it1) << std::endl;
}