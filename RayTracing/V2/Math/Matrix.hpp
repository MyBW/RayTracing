#pragma once
#include<iostream>
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
}