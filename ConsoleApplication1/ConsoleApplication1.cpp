#include <iostream>
#include "omni_map.h"

int main()
{
	util::omni_map<int, std::string> test_map;
	test_map.emplace(3, "three");
	test_map.emplace(2, "two");
	test_map.emplace(7, "seven");
	test_map.emplace(1, "one");
	test_map.emplace(6, "six");
	test_map.emplace(4, "four");
	test_map.emplace(5, "five");
	auto it0 = test_map.find<0>(4);
	auto it1 = test_map.find<1>("seven");
	std::cout << std::get<0>(*it0) << " " << std::get<1>(*it0) << std::endl;
	std::cout << std::get<0>(*it1) << " " << std::get<1>(*it1) << std::endl;
	test_map.erase(it0);
	it0 = test_map.find<0>(4);
	it1 = test_map.find<1>("four");
	std::cout << (it0 == test_map.end<0>()) << std::endl;
}