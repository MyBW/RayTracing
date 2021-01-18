#pragma once
#include"gtest/gtest.h"
#include "..\V2\Filter.hpp"
using namespace BlackWalnut;
TEST(Filter, ZeroPastRadius) 
{
	std::vector<Vector2f> Points({ Vector2f(1, 1), Vector2f(1.5, .25), Vector2f(.33, 5.2),
		Vector2f(.1, .1), Vector2f(3, 3) });
	for (Vector2f& r : Points)
	{
		std::vector<FilterBase*> Filters{ new BoxFilter(r)};
		for (FilterBase*F: Filters)
		{
			FilterBase& f = *F;
			EXPECT_EQ(0, f.Evaluate(Vector2f(0, r.Y + 1e-3)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(r.X, r.Y + 1e-3)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(-r.X, r.Y + 1e-3)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(0, -r.Y - 1e-3)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(r.X, -r.Y - 1e-3)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(-r.X, -r.Y - 1e-3)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(r.X + 1e-3, 0)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(r.X + 1e-3, r.Y)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(r.X + 1e-3, -r.Y)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(-r.X - 1e-3, 0)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(-r.X - 1e-3, r.Y)));
			EXPECT_EQ(0, f.Evaluate(Vector2f(-r.X - 1e-3, -r.Y)));
		}
	}
}