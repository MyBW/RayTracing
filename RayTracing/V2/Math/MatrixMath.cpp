#include "MatrixMath.hpp"
namespace BlackWalnut
{

	float FMA(float a, float b, float c)
	{
		return std::fma(a, b, c);
	}
	float Determinant(Matrix3X3f m)
	{
		float minor12 = DifferenceOfProducts(m[1][1], m[2][2], m[1][2], m[2][1]);
		float minor02 = DifferenceOfProducts(m[1][0], m[2][2], m[1][2], m[2][0]);
		float minor01 = DifferenceOfProducts(m[1][0], m[2][1], m[1][1], m[2][0]);
		return FMA(m[0][2], minor01,
			DifferenceOfProducts(m[0][0], minor12, m[0][1], minor02));
	}

	BlackWalnut::Matrix3X3f LinearLeastSquares(const float A[][3], const float B[][3], int rows)
	{
		Matrix3X3f AtA, AtB;
		AtA.MakeZero();
		AtB.MakeZero();

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				for (int r = 0; r < rows; ++r)
				{
					AtA[i][j] += A[r][i] * A[r][j];
					AtB[i][j] += A[r][i] * B[r][j];
				}
			}
		}
		auto AtAi = Inverse(AtA);
		Matrix3X3f Result;
		Transpose(Result, AtAi * AtB);
		return Result;
	}
}