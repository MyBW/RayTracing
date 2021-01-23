#pragma once
#include <assert.h>
#include <iostream>
namespace BlackWlanut
{
#define CHECK(X) assert(X)
#define CHECK_IMP(A,B,Op) assert((A)Op(B))

#define CHECK_EQ(A,B) CHECK_IMP(A,B,==)
#define CHECK_NE(A,B) CHECK_IMP(A,B,!=)
#define CHECK_GE(A,B) CHECK_IMP(A,B,>=)
#define CHECK_LT(A,B) CHECK_IMP(A,B,<)
#define CHECK_GT(A,B) CHECK_IMP(A,B,>);
#define CHECK_LE(A,B) CHECK_IMP(A,B,<=);
#define WARN(A) std::cout << A << std::endl;
}
