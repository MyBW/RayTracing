#pragma once 
#include"Geom.hpp"
#include"Matrix.hpp"

namespace BlackWalnut
{
	
	

    template<typename T>
    void Matrix3X3MultVector3( const Vector3Type<T>& Result, const Matrix<T, 3, 3>& M, const Vector3Type<T>& V)
    {
        Result.MakeZero();
        for(int i = 0 ; i < 3; i++)
        {
            for(int j = 0 ; j < 3; j++)
            {
                Result.Data[i] += M.Data[i][j] * V.Data[j];
            }
        }
    }
    template<typename T>
    void Matrix4X4MultVector4(Vector4Type<T>& Result, const Matrix<T, 4, 4>& M, const Vector4Type<T>& V)
    {
        Result.MakeZero();
        for(int i = 0 ; i < 4; i++)
        {
            for(int j = 0 ; j < 4; j++)
            {
                Result.Data[i] += M.Data[i][j] * V.Data[j];
            }
        }
    }
    template<typename T>
    void Matrix4X4MultVector3( Vector3Type<T>& Result, const Matrix<T, 4, 4>& M, const Vector3Type<T>& V, const T W = T(1.0))
    {
        Vector4Type<T> V4(V.X, V.Y, V.Z, W);
        Vector4Type<T> R;
        Matrix4X4MultVector4(R, M, V4);
		Result.X = R.X; Result.Y = R.Y; Result.Z = R.Z;
    }

    template<typename T>
    void MakeViewMatrix(Matrix<T, 4, 4>& Result, const Vector3Type<T>& Position, const Vector3Type<T>& LookAt, const Vector3Type<T>& Up)
    {
        Vector3Type<T> XAxis, YAxis, ZAxis;
        T Result1 = T(0), Result2 = T(0), Result3 = T(0);
        ZAxis = Position - LookAt;
        Normalize(ZAxis);
		CrossProduct(XAxis, Up, ZAxis);
        Normalize(XAxis);
        CrossProduct(YAxis, ZAxis, XAxis);

        DotProduct(Result1, XAxis, Position);
        Result1 = -Result1;

        DotProduct(Result2, YAxis, Position);
        Result2 = -Result2;

        DotProduct(Result3, ZAxis, Position);
        Result3 = -Result3;

        Matrix<T, 4, 4> Tmp = {{{
            { XAxis.X, YAxis.X, ZAxis.X, T(0.0) },
            { XAxis.Y, YAxis.Y, ZAxis.Y, T(0.0)},
            { XAxis.Z, YAxis.Z, ZAxis.Z, T(0.0) },
            { Result1, Result2, Result3, T(1.0) }
        }}};
        Transpose(Result, Tmp);
    }
    template<typename T>
    void MakeOrthograhpicMatrix(Matrix<T, 4, 4>& ProjectMatrix, T Top, T Bottom, T Right, T Left, T Near, T Far)
    {
        T inv_w = T(1.0) / (Right - Left);
		T inv_h = T(1.0) / (Top - Bottom);
		T inv_d = T(1.0) / (Far - Near);

        T A = 2 * inv_w;
		T B = 2 * inv_h;
		T C = -(Right + Left) *inv_w;
		T D = -(Top + Bottom) * inv_h;
		T q, qn;
        q = -2 * inv_d;
		qn = -(Far + Near) * inv_d;

        // NB: This creates 'uniform' orthographic projection matrix,
			// which depth range [-1,1], right-handed rules
			//
			// [ A   0   0   C  ]
			// [ 0   B   0   D  ]
			// [ 0   0   q   qn ]
			// [ 0   0   0   1  ]
			//
			// A = 2 * / (right - left)
			// B = 2 * / (top - bottom)
			// C = - (right + left) / (right - left)
			// D = - (top + bottom) / (top - bottom)
			// q = - 2 / (far - near)
			// qn = - (far + near) / (far - near)
        	ProjectMatrix.MakeZero();
			ProjectMatrix[0][0] = A;
			ProjectMatrix[0][3] = C;
			ProjectMatrix[1][1] = B;
			ProjectMatrix[1][3] = D;
			ProjectMatrix[2][2] = q;
			ProjectMatrix[2][3] = qn;
			ProjectMatrix[3][3] = 1;
    }
    template<typename T>
    void MakeProjectMatrix(Matrix<T, 4, 4>& ProjectMatrix, T Top, T Bottom, T Right, T Left, T Near, T Far)
    {
        T inv_w = T(1.0) / (Right - Left);
		T inv_h = T(1.0) / (Top - Bottom);
		T inv_d = T(1.0) / (Far - Near);
        T A = 2 * Near * inv_w;
		T B = 2 * Near * inv_h;
		T C = (Right + Left) * inv_w;
		T D = (Top + Bottom) * inv_h;
		T q, qn;
		q = -(Far + Near) * inv_d;
		qn = -2 * (Far * Near) * inv_d;
			// NB: This creates 'uniform' perspective projection matrix,
			// which depth range [-1,1], right-handed rules
			//
			// [ A   0   C   0  ]
			// [ 0   B   D   0  ]
			// [ 0   0   q   qn ]
			// [ 0   0   -1  0  ]
			//
			// A = 2 * near / (right - left)
			// B = 2 * near / (top - bottom)
			// C = (right + left) / (right - left)
			// D = (top + bottom) / (top - bottom)
			// q = - (far + near) / (far - near)
			// qn = - 2 * (far * near) / (far - near)
		ProjectMatrix.MakeZero();
		ProjectMatrix[0][0] = A;
		ProjectMatrix[0][2] = C;
		ProjectMatrix[1][1] = B;
		ProjectMatrix[1][2] = D;
		ProjectMatrix[2][2] = q;
		ProjectMatrix[2][3] = qn;
		ProjectMatrix[3][2] = -1;

		//dx project matrix
			// NB: This creates 'uniform' perspective projection matrix,
			// which depth range [0,1], left-handed rules
			//
			// [ A   0   C   0  ]
			// [ 0   B   D   0  ]
			// [ 0   0   q   qn ]
			// [ 0   0   1  0  ]
			//
			// A = 2 * near / (right - left)
			// B = 2 * near / (top - bottom)
			// C = (right + left) / (right - left)
			// D = (top + bottom) / (top - bottom)
			// q = far / (far - near)
			// qn = - (far * near) / (far - near)
		/*{
			T A = 2 * Near * inv_w;
			T B = 2 * Near * inv_h;
			T C = (Right + Left) * inv_w;
			T D = (Top + Bottom) * inv_h;
			T q, qn;
			q = (Far)* inv_d;
			qn = -(Far * Near) * inv_d;

			ProjectMatrix.MakeZero();
			ProjectMatrix[0][0] = A;
			ProjectMatrix[0][2] = C;
			ProjectMatrix[1][1] = B;
			ProjectMatrix[1][2] = D;
			ProjectMatrix[2][2] = q;
			ProjectMatrix[2][3] = qn;
			ProjectMatrix[3][2] = 1;
		}*/
		

    }

	template<typename T>
	void MakeProjectMatrixLHDX(Matrix<T, 4, 4>& ProjectMatrix, T Top, T Bottom, T Right, T Left, T Near, T Far)
	{
		//dx project matrix
		   // NB: This creates 'uniform' perspective projection matrix,
		   // which depth range [0,1], left-handed rules
		   //
		   // [ A   0   C   0  ]
		   // [ 0   B   D   0  ]
		   // [ 0   0   q   qn ]
		   // [ 0   0   1  0  ]
		   //
		   // A = 2 * near / (right - left)
		   // B = 2 * near / (top - bottom)
		   // C = (right + left) / (right - left)
		   // D = (top + bottom) / (top - bottom)
		   // q = far / (far - near)
		   // qn = - (far * near) / (far - near)
		T inv_w = float(1.0) / (Right - Left);
		T inv_h = float(1.0) / (Top - Bottom);
		T inv_d = float(1.0) / (Far - Near);
		T A = 2 * Near * inv_w;
		T B = 2 * Near * inv_h;
		T C = (Right + Left) * inv_w;
		T D = (Top + Bottom) * inv_h;
		T q, qn;
		q = (Far)* inv_d;
		qn = -(Far * Near) * inv_d;
		ProjectMatrix.MakeZero();
		ProjectMatrix[0][0] = A;
		ProjectMatrix[0][2] = C;
		ProjectMatrix[1][1] = B;
		ProjectMatrix[1][2] = D;
		ProjectMatrix[2][2] = q;
		ProjectMatrix[2][3] = qn;
		ProjectMatrix[3][2] = 1;
	}
	template<typename T>
	void MakeProjectMatrixLHDX(Matrix<T, 4, 4>& ProjectMatrix, T FOV, T Aspect, T Near, T Far)
	{
		ProjectMatrix.MakeZero();
		ProjectMatrix[0][0] = 1 / (Aspect*tan(FOV / 2));
		ProjectMatrix[1][1] = 1 / tan(FOV / 2);
		ProjectMatrix[2][2] = Far / (Far - Near);
		ProjectMatrix[2][3] = -Far * Near / (Far - Near);
		ProjectMatrix[3][2] = 1;
	}
	template<typename T>
	void MakeInvProjectMatrixLHDX(Matrix<T, 4, 4>& ProjectMatrix, T FOV, T Aspect, T Near, T Far)
	{
		T a = 1 / (Aspect*tan(FOV / 2));
		T b = 1 / tan(FOV / 2);
		T c = Far / (Far - Near);
		T d = -Far * Near / (Far - Near);
		ProjectMatrix.MakeZero();
		ProjectMatrix[0][0] = d / a;
		ProjectMatrix[1][1] = d / b;
		ProjectMatrix[2][3] = d;
		ProjectMatrix[3][2] = 1;
		ProjectMatrix[3][3] = -c;
	}
    template<typename T>
    void MakeRotateMatrix(Matrix<T, 4, 4>& RotateMatrix, float Theta, T X, T Y, T Z)
    {
        T Mod = sqrt(X*X + Y*Y + Z*Z);
        X = X / Mod;
        Y = Y / Mod;
        Z = Z / Mod;
        T X2 = X * X;
        T Y2 = Y * Y;
        T Z2 = Z * Z;

        Theta = (Theta * PI) / T(180.0);
        T C = Check_Zero(cos(Theta));
        T S = Check_Zero(sin(Theta));
        RotateMatrix[0][0] = X2 + (1 - X2)*C;   RotateMatrix[0][1] = X*Y*(1 - C) - Z * S;   RotateMatrix[0][2] = X*Z*(1 - C) + Y*S;    RotateMatrix[0][3] = T(0);
	    RotateMatrix[1][0] = X*Y*(1 - C) + Z*S; RotateMatrix[1][1] = Y2 + (1 - Y2)*C;       RotateMatrix[1][2] = Y*Z*(1 - C) - X *S;   RotateMatrix[1][3] = T(0);
	    RotateMatrix[2][0] = X*Z*(1 - C) - Y*S; RotateMatrix[2][1] = Y*Z*(1 - C) + X*S;     RotateMatrix[2][2] = Z2 + (1 - Z2)*C;      RotateMatrix[2][3] = T(0);
	    RotateMatrix[3][0] = T(0);              RotateMatrix[3][1] = T(0);                  RotateMatrix[3][2] = T(0);                 RotateMatrix[3][3] = T(1);

	/* reslut[0] = x2 + (1 - x2)*c;   reslut[4] = x*y*(1 - c) - z * s;   reslut[8] = x*z*(1 - c) + y*s;    reslut[12] = 0;
	 reslut[1] = x*y*(1 - c) + z*s; reslut[5] = y2 + (1 - y2)*c;       reslut[9] = y*z*(1 - c) - x *s;   reslut[13] = 0;
	 reslut[2] = x*z*(1 - c) - y*s; reslut[6] = y*z*(1 - c) + x*s;     reslut[10] = z2 + (1 - z2)*c;     reslut[14] = 0;
	 reslut[3] = 0;                reslut[7] = 0;                     reslut[11] = 0;                    reslut[15] = 1;*/
    }
    template<typename T>
    void MakeScaleMatrix(Matrix<T, 4, 4>& ScaleMatrix, T X, T Y, T Z)
    {
        ScaleMatrix.MakeZero();
        ScaleMatrix[0][0] = X;
        ScaleMatrix[1][1] = Y;
        ScaleMatrix[2][2] = Z;
        ScaleMatrix[3][3] = T(1.0);
    }
    template<typename T>
    void MakeTranslateMatrix(Matrix<T, 4, 4>& TranslateMatrix, T X, T Y, T Z)
    {
        TranslateMatrix.MakeZero();
        TranslateMatrix[0][0] = T(1);
        TranslateMatrix[1][1] = T(1);
        TranslateMatrix[2][2] = T(1);
        TranslateMatrix[3][3] = T(1);
        TranslateMatrix[0][3] = X; TranslateMatrix[1][3] = Y; TranslateMatrix[2][3] = Z;
    }
	template<typename T>
	void ExchangeYAndZ(Matrix<T, 4, 4>& InMatrix)
	{
		for (int i = 0; i < 4; i++)
		{
			T Tmp = InMatrix[1][i];
			InMatrix[1][i] = InMatrix[2][i];
			InMatrix[2][i] = Tmp;
		}
	}
	template<typename T>
	 void InverseMatrix(Matrix<T, 4, 4>&Reslut, Matrix<T, 4, 4>& _M)
	{
		// 并非所有的矩阵都有逆矩阵。然而所有的仿射矩阵都有逆矩阵，可用高斯消去法或LU分解求之。
		float m00 = _M[0][0], m01 = _M[0][1], m02 = _M[0][2], m03 = _M[0][3];
		float m10 = _M[1][0], m11 = _M[1][1], m12 = _M[1][2], m13 = _M[1][3];
		float m20 = _M[2][0], m21 = _M[2][1], m22 = _M[2][2], m23 = _M[2][3];
		float m30 = _M[3][0], m31 = _M[3][1], m32 = _M[3][2], m33 = _M[3][3];

		float v0 = m20 * m31 - m21 * m30;
		float v1 = m20 * m32 - m22 * m30;
		float v2 = m20 * m33 - m23 * m30;
		float v3 = m21 * m32 - m22 * m31;
		float v4 = m21 * m33 - m23 * m31;
		float v5 = m22 * m33 - m23 * m32;

		float t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
		float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
		float t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
		float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

		float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

		float d00 = t00 * invDet;
		float d10 = t10 * invDet;
		float d20 = t20 * invDet;
		float d30 = t30 * invDet;

		float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m10 * m31 - m11 * m30;
		v1 = m10 * m32 - m12 * m30;
		v2 = m10 * m33 - m13 * m30;
		v3 = m11 * m32 - m12 * m31;
		v4 = m11 * m33 - m13 * m31;
		v5 = m12 * m33 - m13 * m32;

		float d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m21 * m10 - m20 * m11;
		v1 = m22 * m10 - m20 * m12;
		v2 = m23 * m10 - m20 * m13;
		v3 = m22 * m11 - m21 * m12;
		v4 = m23 * m11 - m21 * m13;
		v5 = m23 * m12 - m22 * m13;

		float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;
		
		Reslut[0][0] = d00; Reslut[0][1] = d01; Reslut[0][2] = d02; Reslut[0][3] = d03;
		Reslut[1][0] = d10; Reslut[1][1] = d11; Reslut[1][2] = d12; Reslut[1][3] = d13;
		Reslut[2][0] = d20; Reslut[2][1] = d21; Reslut[2][2] = d22; Reslut[2][3] = d23;
		Reslut[3][0] = d30; Reslut[3][1] = d31; Reslut[3][2] = d32; Reslut[3][3] = d33;
	
	}



	 //PBRT_Math

	 
	 float FMA(float a, float b, float c);

	 template <typename Ta, typename Tb, typename Tc, typename Td>
	 inline auto DifferenceOfProducts(Ta a, Tb b, Tc c, Td d) {
		 auto cd = c * d;
		 auto differenceOfProducts = FMA(a, b, -cd);
		 auto error = FMA(-c, d, cd);
		 return differenceOfProducts + error;
	 }
	 
	 float Determinant(Matrix3X3f m);
		
	 Matrix3X3f Inverse3x3f(const Matrix3X3f &m);
	 Matrix3X3f LinearLeastSquares(const float A[][3], const float B[][3], int rows);
	

	 template<typename TResult, int N , typename T>
	 inline TResult Mul(const Matrix3X3f &m, const T& v)
	 {
		 TResult result;
		 for (int i = 0; i < N; ++i) {
			 result[i] = 0;
			 for (int j = 0; j < N; ++j)
				 result[i] += m[i][j] * v[j];
		 }
		 return result;
	 }


	 template <typename C>
	 inline constexpr float EvaluatePolynomial(float t, C c) {
		 return c;
	 }
	 template<typename C, typename... Args>
	 constexpr float EvaluatePolynomial(float T, C InC, Args... CRemaining)
	 {
		 using FMAT = typename std::common_type<float, C>::type;
		 return FMA(FMAT(T), FMAT(EvaluatePolynomial(T, CRemaining...)), FMAT(InC));
	 }
}