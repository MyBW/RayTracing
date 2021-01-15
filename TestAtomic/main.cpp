#include <iostream>
#include <atomic>
#include <vector>

struct TestStruct
{
	TestStruct() = default;
	TestStruct(const TestStruct& T)
	{

	}
	std::atomic<double> TestAry[3];
};
std::vector<std::vector<TestStruct>> Test2D;

void main()
{
	std::atomic<double> test = 10;
	std::atomic<double> &test1 = test;
	test1 = test1 + 10;

	std::atomic<double> TestAry[3];

	

	std::atomic<double>& T = TestAry[0];

	Test2D.resize(10);

	{
		auto&Test2DElement = Test2D[1];
		Test2DElement.resize(10);
	}
	

	//for (auto&Test2DElement : Test2D)
	//{
	//	Test2DElement.resize(10);
	//}

	std::cout << "hello " << Test2D[2][2].TestAry[1] << std::endl;

	std::cout << "hello " << T <<std::endl;
	std::cout << "hello " << test1 << std::endl;
}