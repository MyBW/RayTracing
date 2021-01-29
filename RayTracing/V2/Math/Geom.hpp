#pragma once
#include<math.h>
#include<iostream>
#include <memory.h>
#pragma warning(disable : 4201)
#ifndef PI
#define PI 3.14159265358979323846f
#endif
#include "..\V2\Util\Check.h"
#include <algorithm>
#include "Math.hpp"

#ifndef TWO_PI
#define TWO_PI 3.14159265358979323846f * 2.0f
#endif
#ifndef ESP
#define ESP 1e-7
#endif
#ifndef Check_Zero
#define Check_Zero(V) ((abs(V)<ESP)?(0.0f):(V))
#endif // !Check_Zero

namespace BlackWalnut
{
    template<typename T, size_t SizeOfArray>
    constexpr size_t CountOf(T(&)[SizeOfArray]) { return SizeOfArray;};

    template<typename T, int ...Indexes>
    class Swizzle
    {
        private:
            float V[sizeof...(Indexes)];
        public:
            T& operator=(const T& Rhs)
            {
                int Idx [] ={...Indexes}; 
                for(int i = 0 ;i < sizeof...(Indexes); i++)
                {
                    V[Idx[i]] = Rhs[i];
                }
                return *(T*)this;
            }
			T& operator+(const T&Rhs)
			{
				int Idx[] = { ...Indexes };
				for (int i = 0; i < sizeof...(Indexes); i++)
				{
					V[Idx[i]] += Rhs[i];
				}
				return *(T*)this;
			}
			T& operator-(const T&Rhs)
			{
				int Idx[] = { ...Indexes };
				for (int i = 0; i < sizeof...(Indexes); i++)
				{
					V[Idx[i]] -= Rhs[i];
				}
				return *(T*)this;
			}
            operator T () const 
            {
                return T(V[Indexes]...);
            }
    };

    template<typename T>
	inline constexpr T clamp(T v, T min, T max) noexcept 
	{
		return T(
			(min > v ? min : v) > max ? max : (min > v ? min : v)
		);
		//return T(min(max, max(min, v)));
	}
    
    template<typename T>
    struct Vector2Type
    {
        union 
        {
            T Data[2];
            struct { T X,Y; };
            struct { T U,V; };
            struct { T R,G; };
            Swizzle<Vector2Type<T>, 0, 1> XY;
            Swizzle<Vector2Type<T>, 1, 0> YX;
        };
        Vector2Type<T>(){};
        Vector2Type<T>(const T &V):X(V),Y(V){};
        Vector2Type<T>(const T &InX, const T &InY):X(InX),Y(InY){};
        void MakeZero(){ memset(Data, 0, sizeof(*this));}
        operator T*() { return Data; };
        operator const T*() const { return static_cast<const T*>(Data); };

		bool operator==(const Vector2Type &Other) const
		{
			return Data[0] == Other.Data[0] && Data[1] == Other.Data[1];
		}
    };

    template<typename T>
	struct Vector3Type
	{
		union
		{
			T Data[3];
			struct { T X, Y, Z; };
			struct { T R, G, B; };
			struct { T U, V, W; };
			Swizzle<Vector2Type<T>, 0, 1> XY;
			Swizzle<Vector2Type<T>, 1, 0> YX;
			Swizzle<Vector2Type<T>, 0, 2> XZ;
			Swizzle<Vector2Type<T>, 2, 0> ZX;
			Swizzle<Vector2Type<T>, 1, 2> YZ;
			Swizzle<Vector2Type<T>, 2, 1> ZY;
			Swizzle<Vector3Type<T>, 0, 1, 2> XYZ;
			Swizzle<Vector3Type<T>, 1, 0, 2> YXZ;
			Swizzle<Vector3Type<T>, 0, 2, 1> XZY;
			Swizzle<Vector3Type<T>, 2, 0, 1> ZXY;
			Swizzle<Vector3Type<T>, 1, 2, 0> YZX;
			Swizzle<Vector3Type<T>, 2, 1, 0> ZYX;
		};
		Vector3Type<T>() {};
		Vector3Type<T>(const T& V) : X(V), Y(V), Z(V) {};
		Vector3Type<T>(const T& x, const T& y, const T& z) : X(x), Y(y), Z(z) {};
		operator T*() { return Data; };
		void MakeZero() { memset(Data, 0, sizeof(*this)); }
		operator const T*() const { return static_cast<const T*>(Data); };
		Vector3Type<T> operator/(float Value) const
		{
			CHECK(Value);
			Vector3Type<T> Ret;
			Ret.X = X / Value;
			Ret.Y = Y / Value;
			Ret.Z = Z / Value;
			return Ret;
		}
		Vector3Type<T> operator*(float Value) const
		{
			CHECK(Value);
			Vector3Type<T> Ret;
			Ret.X = X * Value;
			Ret.Y = Y * Value;
			Ret.Z = Z * Value;
			return Ret;
		}
		Vector3Type<T> operator-(Vector3Type<T>& V)
		{
			Vector3Type<T> Ret;
			Ret.X = X - V.X;
			Ret.Y = Y - V.Y;
			Ret.Z = Z - V.Z;
			return Ret;
		}
		bool IsNaN() const
		{
			using BlackWalnut::IsNaN;
			return IsNaN(X) || IsNaN(Y) || IsNaN(Z);
		}
    };
    
    template <typename T>
    struct Vector4Type
    {
        union {
            T Data[4];
            struct { T X, Y, Z, W; };
            struct { T R, G, B, A; };
			Swizzle<Vector3Type<T>, 0, 1, 2> XYZ;
		    Swizzle<Vector3Type<T>, 0, 2, 1> XZY;
		    Swizzle<Vector3Type<T>, 1, 0, 2> YXZ;
		    Swizzle<Vector3Type<T>, 1, 2, 0> YZX;
		    Swizzle<Vector3Type<T>, 2, 0, 1> ZXY;
		    Swizzle<Vector3Type<T>, 2, 1, 0> ZYX;
        };
        Vector4Type<T>() {};
        Vector4Type<T>(const T& V) : X(V), Y(V), Z(V), W(V) {};
        Vector4Type<T>(const T& InX, const T& InY, const T& InZ, const T& InW) : X(InX), Y(InY), Z(InZ), W(InW) {};
        Vector4Type<T>(const Vector3Type<T>& V3) : X(V3.X), Y(V3.Y), Z(V3.Z), W(1.0f) {};
        Vector4Type<T>(const Vector3Type<T>& V3, const T& InW) : X(V3.X), Y(V3.Y), Z(V3.Z), W(InW) {};
		Vector4Type<T>(const Vector4Type<T>& V4) : X(V4.X), Y(V4.Y), Z(V4.Z), W(V4.W) { };
        void MakeZero(){ memset(Data, 0, sizeof(*this));}
        operator T*() { return Data; };
        operator const T*() const { return static_cast<const T*>(Data); };
        Vector4Type& operator=(const T* Data)
        {
            for(int i= 0 ;i < 4; i++)
            {  
                this->Data[i] = Data[i];
            }
            return *this;
        }
		Vector4Type& operator=(const Vector4Type<T>& Rhs)
		{
			for (int i = 0; i < 4; i++)
			{
				this->Data[i] = Rhs.Data[i];
			}
			return *this;
		}
    };

    template<template<typename> class TT, typename T>
    std::ostream& operator<<(std::ostream& Out, TT<T> Vector)
    {
        Out << "( ";
        for( size_t i = 0;i < CountOf(Vector.Data); i++)
        {
            Out << Vector.Data[i] << ((i == CountOf(Vector.Data) - 1) ? " " : ", ");
        }
        Out << ")";
        return Out;
    };

    template<template<typename> class TT, typename T>
    void VectorAdd(TT<T> &Result, const TT<T> &Vector1, const TT<T> &Vector2)
    {
        for(size_t i = 0 ;i < CountOf(Result.Data); i++)
        {
            Result.Data[i] = Vector1.Data[i] + Vector2.Data[i];
        }
    }

    template<template<typename> class TT, typename T>
    TT<T> operator+(const TT<T> &Vector1, const TT<T> &Vecotr2)
    {
        TT<T> Result;
        VectorAdd(Result, Vector1, Vecotr2);
        return Result;
    }

    template<template<typename> class TT, typename T>
    void VectorSub(TT<T> &Result, const TT<T> &Vector1, const TT<T> &Vector2)
    {
        for(size_t i = 0; i < CountOf(Result.Data); i++)
        {
            Result.Data[i] = Vector1.Data[i] - Vector2.Data[i];
        }
    }

    template<template<typename> class TT, typename T>
    TT<T> operator-(const TT<T>& Vector1, const TT<T>& Vector2 )
    {
        TT<T> Result;
        VectorSub(Result, Vector1, Vector2);
        return Result;
    }

    template<template<typename> class TT, typename T>
    void CrossProduct(TT<T>& Result, const TT<T>& Vector1, const TT<T>& Vector2)
    {
		Result.X = Vector1.Y * Vector2.Z - Vector2.Y * Vector1.Z;
		Result.Y = Vector1.Z * Vector2.X - Vector2.Z * Vector1.X;
		Result.Z = Vector1.X * Vector2.Y - Vector2.X * Vector1.Y;
    }
    template<template<typename> class TT, typename T>
    void DotProduct(T& Result, const TT<T>& Vector1, const TT<T>& Vector2)
    {
        for(size_t i = 0 ;i < CountOf(Vector1.Data); i++)
        {
            Result += Vector1.Data[i] * Vector2.Data[i];
        }
	}
	template<template<typename> class TT, typename T>
	T Dot(const TT<T>& Vector1, const TT<T>& Vector2)
	{
		T Result = (T)0;
		for (size_t i = 0; i < CountOf(Vector1.Data); i++)
		{
			Result += Vector1.Data[i] * Vector2.Data[i];
		}
		return Result;
	}
	template<template<typename> class TT, typename T>
	T AbsDot(const TT<T>& Vector1, const TT<T>& Vector2)
	{
		T Result = (T)0;
		for (size_t i = 0; i < CountOf(Vector1.Data); i++)
		{
			Result += Vector1.Data[i] * Vector2.Data[i];
		}
		return std::abs(Result);
	}
	/*template<template<typename> class TT, typename T>
	TT<T> operator*(const TT<T>& Vector1, const T Value)
	{
		TT<T> Reslut;
		Reslut.X = Vector1.X  * Value;
		Reslut.Y = Vector1.Y  * Value;
		Reslut.Z = Vector1.Z  * Value;
		return Reslut;
	}*/
	template<template<typename> class TT, typename T>
	TT<T> operator*(const TT<T>& Vector1, const T Value)
	{
		TT<T> Result;
		for (int i = 0; i < CountOf(Result.Data); i++)
		{
			Result[i] = Vector1[i] * Value;
		}
		return Result;
	}
	template<template<typename> class TT, typename T>
	TT<T> operator/(const TT<T>& Vector1, const T Value)
	{
		TT<T> Result;
		for (int i = 0;i < CountOf(Result.Data);i++)
		{
			Result[i] = Vector1[i] / Value;
		}
		return Result;
	}
    template<template<typename> class TT, typename T>
    T Length(const TT<T> &Result)
    {
        T Result2 = (T)0;
        for(size_t i = 0 ;i < CountOf(Result.Data); i++)
        {
            Result2 += Result.Data[i] * Result.Data[i];
        }
        return sqrt(Result2);
    }
    template<template<typename> class TT, typename T>
    void Normalize(TT<T> &Result)
    {
        T L = Length(Result);
        for(size_t i = 0 ;i < CountOf(Result.Data); i++)
        {
            Result.Data[i] = Result.Data[i] / L; 
        }
    }
	template<template<typename> class TT, typename T>
	TT<T> VectorAbs(TT<T> &Input)
	{
		TT<T> Result;
		for (size_t i = 0; i < CountOf(Result.Data); i++)
		{
			Result.Data[i] = std::abs(Input.Data[i]);
		}
		return Result;
	}
	template<template<typename> class TT, typename T>
	TT<T> VectorMax(TT<T> &Input1, TT<T> &Input2)
	{
		TT<T> Result;
		for (size_t i = 0; i < CountOf(Result.Data); i++)
		{
			Result.Data[i] = std::max(Input1.Data[i], Input2.Data[i]);
		}
		return Result;
	}
	template<typename T1, typename T>
	inline Vector3Type<T> operator*(T1 a, Vector3Type<T> Vec3)
	{
		Vector3Type<T> Res;
		for (size_t i = 0; i < CountOf(Res.Data); i++)
		{
			Res.Data[i] = Vec3.Data[i] * a;
		}
		return Res;
	}

	typedef Vector2Type<float> Vector2f;
	typedef Vector3Type<float> Vector3f;
	typedef Vector4Type<float> Vector4f;
	typedef Vector4Type<double> Vector4d;
    typedef Vector4Type<uint8_t> R8G8B8Unorm;
	typedef Vector2Type<int32_t> Vector2i;

	inline float AbsCosTheta(const Vector3f &w) {
		return std::abs(w.Z);
	}
	inline bool SameHemisphere(const Vector3f &Wi, const Vector3f &Wo)
	{
		return Wi.Z * Wo.Z > 0.0f;
	}
	
	inline float Cos2Theta(const Vector3f &w) 
	{
		return w.Z * w.Z;
	}
	inline float Sin2Theta(const Vector3f &w) {
		return std::max<float>(0, 1 - Cos2Theta(w));
	}
	inline float SinTheta(const Vector3f &w) {
		return std::sqrt(Sin2Theta(w));
	}
	inline float CosDPhi(const Vector3f &wa, const Vector3f &wb) {
		float waxy = wa.X * wa.X + wa.Y * wa.Y;
		float wbxy = wb.X * wb.X + wb.Y * wb.Y;
		if (waxy == 0 || wbxy == 0)
			return 1;
		return Clamp((wa.X * wb.X + wa.Y * wb.Y) / std::sqrt(waxy * wbxy), -1.0f, 1.0f);
	}

	inline float CosPhi(const Vector3f &w) 
	{
		float sinTheta = SinTheta(w);
		return (sinTheta == 0) ? 1 : Clamp(w.X / sinTheta, -1.0f, 1.0f);
	}

	inline float SinPhi(const Vector3f &w) 
	{
		float sinTheta = SinTheta(w);
		return (sinTheta == 0) ? 0 : Clamp(w.Y / sinTheta, -1.0f, 1.0f);
	}


	inline float Tan2Theta(const Vector3f &w) 
	{
		return Sin2Theta(w) / Cos2Theta(w);
	}
	inline float Cos2Phi(const Vector3f &w) 
	{
		return CosPhi(w) * CosPhi(w);
	}
	inline float Sin2Phi(const Vector3f &w) 
	{
		return SinPhi(w) * SinPhi(w);
	}

	inline Vector3f FaceForward(const Vector3f &v, const Vector3f &v2) 
	{
		return (Dot(v, v2) < 0.f) ? v * -1.0f : v;
	}
}

