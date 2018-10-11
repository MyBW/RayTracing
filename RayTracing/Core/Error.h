#pragma once
#include <iostream>

#define Error(expr)\
	((expr) ? (void)0 : \
	 std::cout<<"Assertion "<<#expr<<" failed in " << __FILE__ << ", line " << __LINE__ << std::endl);