#pragma once
#include<iostream>
#include "Geom.hpp"
namespace BlackWalnut
{
	/*
	Matrix 全部使用的是右手坐标系
	*/
    template<typename T, int Rows, int Cols>
    struct Matrix
    {
        union {T Data[Rows][Cols];};
        auto operator[](int RowIndex)
        {
            return Data[RowIndex];
        }
        const auto operator[](int RowIndex) const 
        {
            return Data[RowIndex];
        }
        void MakeZero(){ memset(Data, 0, sizeof(*this));}
        operator T*() { return &Data[0][0]; };
        operator const T*() const { return static_cast<const T*>(&Data[0][0]); };
        Matrix& operator=(const T* Data)
        {
             for (int i = 0; i < Rows; i++) 
             {
                for (int j = 0; j < Cols; j++) {
                    this->Data[i][j] = *(Data + i * Cols + j);
                }
            }
            return *this;
        }
    };

    template <typename T, int ROWS, int COLS>
    std::ostream& operator<<(std::ostream& Out, Matrix<T, ROWS, COLS> Mtx)
    {
        Out << std::endl;
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                Out << Mtx.Data[i][j] << ((j == COLS - 1)? '\n' : ',');
            }
        }
        Out << std::endl;
        return Out;
    }
    template<typename T, int Num>
    void MatrixIdentity(Matrix<T, Num, Num> &Result)
    {
        Result.MakeZero();
        for(int i = 0 ; i < Num; i++)
        {
            Result.Data[i][i] = T(1);
        }
    }
    template<typename T, int Rows, int Cols>
    void MatrixAdd(Matrix<T, Rows, Cols>& Result, const Matrix<T, Rows, Cols>& Matrix1, const Matrix<T, Rows, Cols>& Matrix2)
    {
        for (int i = 0; i < Rows; i++) 
        {
            for (int j = 0; j < Cols; j++) 
            {
                Result.Data[i][j] =  Matrix1.Data[i][j] + Matrix2.Data[i][j];
            }
        }
    }
    template<typename T, int Rows, int Cols>
    Matrix<T, Rows, Cols> operator+(const Matrix<T, Rows, Cols>& Matrix1, const Matrix<T, Rows, Cols> Matrix2)
    {
        Matrix<T, Rows, Cols> Result;
        MatrixAdd(Result, Matrix1, Matrix2);
        return Result;
    }

    template<typename T, int Rows, int Cols>
    void MatrixSub(Matrix<T, Rows, Cols>& Result, const Matrix<T, Rows, Cols>& Matrix1, const Matrix<T, Rows, Cols>& Matrix2)
    {
        for (int i = 0; i < Rows; i++) 
        {
            for (int j = 0; j < Cols; j++) 
            {
                Result.Data[i][j] =  Matrix1.Data[i][j] - Matrix2.Data[i][j];
            }
        }
    }
    template<typename T, int Rows, int Cols>
    Matrix<T, Rows, Cols> operator-(const Matrix<T, Rows, Cols>& Matrix1, const Matrix<T, Rows, Cols> Matrix2)
    {
        Matrix<T, Rows, Cols> Result;
        MatrixSub(Result, Matrix1, Matrix2);
        return Result;
    }
    template <template <typename, int, int> class TT, typename T, int ROWS, int COLS>
    inline void Transpose(TT<T, COLS, ROWS>& Result, const TT<T, ROWS, COLS>& Matrix1)
    {
        for (int i = 0; i < ROWS; i++) 
        {
            for (int j = 0; j < COLS; j++) 
            {
                Result.Data[j][i] =  Matrix1.Data[i][j];
            }
        }
    }
    template <typename T, int Da, int Db, int Dc>
    void MatrixMultiply(Matrix<T, Da, Dc>& Result, const Matrix<T, Da, Db>& Matrix1, const Matrix<T, Dc, Db>& Matrix2)
    {
        Result.MakeZero();
        Matrix<T, Db, Dc> Matrix2_Transpose;
        Transpose(Matrix2_Transpose, Matrix2);
        for (int i = 0; i < Da; i++) 
        {
            for(int k = 0 ; k < Dc; k++)
            {
                for (int j = 0; j < Db; j++) 
                {
                    Result.Data[i][k] += Matrix1.Data[i][j] * Matrix2_Transpose.Data[k][j];
                }
            }
            
        }
    }
    
    template <typename T, int ROWS, int COLS>
    Matrix<T, ROWS, COLS> operator*(const Matrix<T, ROWS, COLS>& matrix1, const Matrix<T, ROWS, COLS>& matrix2)
    {
        Matrix<T, ROWS, COLS> result;
        MatrixMultiply(result, matrix1, matrix2);
        return result;
    }
	typedef Matrix<float, 4, 4> Matrix4X4f;
	typedef Matrix<float, 3, 3> Matrix3X3f;
	typedef Matrix<double, 4, 4> Matrix4X4d;


	template <typename T>
	inline void CoordinateSystem(const Vector3Type<T> &v1, Vector3Type<T> *v2,
		Vector3Type<T> *v3) {
		float sign = std::copysign(float(1), v1.Z);
		float a = -1 / (sign + v1.Z);
		float b = v1.X * v1.Y * a;
		*v2 = Vector3Type<T>(1 + sign * v1.X * v1.X * a, sign * b, -sign * v1.X);
		*v3 = Vector3Type<T>(b, sign + v1.Y * v1.Y * a, -v1.Y);
	}

	class Frame
	{
	public:
		Frame() :X(1.0, 0.0, 0.0), Y(0.0, 1.0, 0.0), Z(0.0, 0.0, 1.0)
		{

		}
		Frame(const Vector3f &X, const Vector3f &Y, const Vector3f &Z) :X(X), Y(Y), Z(Z)
		{
			CHECK(std::abs(Length(X) * Length(X) - 1.0f) < 1e-4);
			CHECK(std::abs(Length(Y) * Length(Y) - 1.0f) < 1e-4);
			CHECK(std::abs(Length(Z) * Length(Z) - 1.0f) < 1e-4);
			float Ret;
			DotProduct(Ret, X, Y);
			CHECK(std::abs(Ret) < 1e-4);
			DotProduct(Ret, X, Z);
			CHECK(std::abs(Ret) < 1e-4);
			DotProduct(Ret, Y, Z);
			CHECK(std::abs(Ret) < 1e-4);
		}
		static Frame FromXY(const Vector3f &X, const Vector3f &Y)
		{
			Vector3f Ret;
			CrossProduct(Ret, X, Y);
			return Frame(X, Y, Ret);
		}
		static Frame FromXZ(const Vector3f &X, const Vector3f &Z)
		{
			Vector3f Ret;
			CrossProduct(Ret, X, Z);
			return Frame(X, Ret, Z);
		}
		static Frame FromZ(const Vector3f &Z)
		{
			Vector3f X, Y;
			CoordinateSystem<float>(Z, &X, &Y);
			return Frame(X, Y, Z);
		}
		Vector3f ToLocal(const Vector3f &V) const 
		{
			float RetX;
			DotProduct(RetX, V, X);
			float RetY;
			DotProduct(RetY, V, Y);
			float RetZ;
			DotProduct(RetZ, V, Z);
			return Vector3f(RetX, RetY, RetZ);
		}
		Vector3f FromLocal(const Vector3f &V) const
		{
			return X * V.X + Y * V.Y + Z * V.Z;
		}
	private:
		Vector3f X, Y, Z;
	};
}