#pragma once
#include <assert.h>
namespace BlackWlanut
{
#define CHECK(X) assert(X)
#define CHECK_IMP(A,B,Op) assert((A)Op(B))

#define CHECK_EQ(A,B) CHECK_IMP(A,B,==)
#define CEHCK_NE(A,B) CHECK_IMP(A,B,!=)
}
