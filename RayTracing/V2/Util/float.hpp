#pragma once

namespace BlackWalnut
{
	//static constexpr float FloatOneMinusEpsilon = 0x1.fffffep-1;  Compile Error In VS2015. P is the "e"(��ѧ��������ָ������) in hexadecimal
	static constexpr float  FloatOneMinusEpsilon = 0.9999999404f;
	static constexpr float OneMinusEpsilon = FloatOneMinusEpsilon;
}