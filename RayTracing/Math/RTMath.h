#pragma once
#include "BWPrimitive.h"
#include <math.h>
#include <float.h>
#include <algorithm>
const float PI = 3.14159265358979323846f;
const float ESP = 0.00001f;//这个调整的话 可能会导致求解凸包过程崩溃
const float FLT_MAX_ =  FLT_MAX;
const float FLT_MIN_ = FLT_MIN;
const float POS_INFINITY = FLT_MAX;
const float INV_PI = 1.0 / 3.14159265358979323846f;
const float INV_TWOPI =  0.15915494309189533577f;

static const float FloatOneMinusEpsilon = 0.99999994;
static const float OneMinusEpsilon = FloatOneMinusEpsilon;


class Math
{
public:
	static double Abs(double value)
	{
		return std::abs(value);
	}
};
void Swap(double& d1, double& d2);
void Swap(float& f1, float& f2);
float Clamp(float Value, float Min, float Max);
int  Rand_int();
float Rand_float();
BWPoint4DD TriangleCentroid(const BWPoint4DD& p1, const BWPoint4DD& p2,  const BWPoint4DD & p3);
BWPoint4DD TriangleCentroid(const BWTriangle& triangle);
float Lerp(float T, float V1, float V2);
double  TriangleArea(const BWTriangle& triangle);
double  TriangleArea(const BWPoint4DD& p1, const BWPoint4DD& p2, const BWPoint4DD & p3) ;
double  Dot(const BWVector3DD& a, const BWVector3DD& b);
float  AbsDot(const BWVector3DD &a, const BWVector3DD &b);
BWVector3DD Cross(const BWVector3DD& a, const BWVector3DD& b);
void    Normalize( BWPoint3DD& vec);
double  Lenth(const BWVector3DD& a);
float DistanceSquared(const BWPoint3DD& P1, const BWPoint3DD &P2);
//WO direction point out from n  . Wo and n is normalize.
BWVector3D Reflect(const BWVector3D &wo, const BWVector3D &n);
BWVector3D GetDirection(const BWVector3D &From, const BWVector3D &To);
const BWVector3DD GetRotatedZ(const BWVector3DD vec, double angle);
const BWVector3DD GetRotatedAxis(const BWVector3DD vec, double angle, const BWVector3DD & axis);
BWPoint4DD MatrixMultPoint(const float* matrix, const BWPoint4DD& point);
BWPoint4DD MatrixMultPoint(const double* matrix, const BWPoint4DD& point );
void MatrixTranspose(const double* matrix, double* trans);
void MatrixTranspose(const float* matrix, float *trans);
void MatrixMultMatrix(const double *matrix1, const double* matrix2, double* reslut);
void MatrixMultMatrix(const double*, double*);

inline BWVector3D SphericalDirection(float sintheta, float costheta, float phi) 
{
	return BWVector3D(sintheta * cosf(phi),
		sintheta * sinf(phi),
		costheta);
}
void CoordinateSystem(const BWVector3D &v1, BWVector3D *v2, BWVector3D *v3);
//
//bool RayIntersectAABB(const BWRay& ray, const Cuboid& AABB , float &t);
//bool RayIntersectOBB(const BWRay& ray , const Cuboid& OBB , float &t);
//射线和三角形形的交点测试  
//t:   相对ray而言
//u,v: 纹理坐标，是对向量p1p2 和 p1p3 
bool RayIntersectTriangle(const BWRay& ray, const BWPoint4DD& p1, const BWPoint4DD &p2, const BWPoint4DD& p3, float &t , float &u, float &v);
//要求都是单位向量,且 ray指向normal 否则返回false
bool ReflcteRay(const BWVector3DD& normal, const BWRay& ray, BWPoint3DD& reflect);
//球面坐标系转化到笛卡尔坐标系
BWPoint4DD SphereCoordToDescartesCoord(float theta, float phi, float radius);
//计算线性差值
inline BWPoint3DD LinearInterpolation(const BWPoint3DD& start, const BWPoint3DD& end, double k)
{
	return (end - start) * k + start;
}
inline BWPoint2DD LinearInterpolation(const BWPoint2DD& start, const BWPoint2DD& end, double k)
{
	return (end - start) * k + start;
}

bool AABBIntersctRay(const BWRay& Ray, const BWVector3D &Min, const BWVector3D &Max);
//注意 在opengl中使用矩阵调用gl内置函数时，其输入方式是列主序的 
//得到的最终矩阵和右手系（OpenGL）矩阵时转置的关系
void GetTranslateMatrix(double x, double y, double z, double * reslut);
void GetRotateMatrix(double theta, float x, float y, float z, double* reslut);
void GetScaleMatrix(float x, float y, float z , double * reslut);
//根据三个向量构建一个坐标系转换矩阵，该矩阵为正交矩阵
void GetMatrixOfThreeVector(const BWPoint3DD& vec1, const BWPoint3DD& vec2, const BWPoint3DD& vec3 , double* reslut);
//分离轴碰撞检测
//bool CollisionDetecte(const Cuboid&, const Cuboid &);

//利用 雅格比 计算对称矩阵的特征值和特征向量
/*
  a:   n*n的对称矩阵
  n:   矩阵维度
  v:   最终的特征向量
  eps: 精度控制
  jt:  最大迭代次数

  返回 -1 代表在该迭代次数下，无法完成该精度的计算
  返回 1  代表正确返回
*/
int Jacobi(double a[], int n, double v[], double eps, int jt);


template<typename T1 , typename T2>
auto TMax(T1 l, T2 r)->decltype(l > r ? l : r)
{
	return l > r ? l : r;
}

 template<typename T1 , typename T2>
 auto TMin(T1 l, T2 r)->decltype(l > r ? r : l)
 {
	 return l > r ? r : l;
 }

 inline unsigned int Hash(const int p[3], int hashSize) {
	 return (unsigned int)((p[0] * 73856093) ^ (p[1] * 19349663) ^
		 (p[2] * 83492791)) %
		 hashSize;
 }