#pragma once
#include "BWPrimitive.h"
#include <math.h>
#include <float.h>
#include <algorithm>
const float PI = 3.1415926f;
const float ESP = 0.0000001f;//��������Ļ� ���ܻᵼ�����͹�����̱���
const float FLT_MAX_ =  FLT_MAX;
const float FLT_MIN_ = FLT_MIN;
const float POS_INFINITY = FLT_MAX;
const float INV_PI = 1.0 / 3.1415926f;



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
BWVector3DD Cross(const BWVector3DD& a, const BWVector3DD& b);
void    Normalize( BWPoint3DD& vec);
double  Lenth(const BWVector3DD& a);
BWVector3D GetDirection(const BWVector3D &From, const BWVector3D &To);
const BWVector3DD GetRotatedZ(const BWVector3DD vec, double angle);
const BWVector3DD GetRotatedAxis(const BWVector3DD vec, double angle, const BWVector3DD & axis);
BWPoint4DD MatrixMultPoint(const float* matrix, const BWPoint4DD& point);
BWPoint4DD MatrixMultPoint(const double* matrix, const BWPoint4DD& point );
void MatrixTranspose(const double* matrix, double* trans);
void MatrixTranspose(const float* matrix, float *trans);
void MatrixMultMatrix(const double *matrix1, const double* matrix2, double* reslut);
void MatrixMultMatrix(const double*, double*);
//
//bool RayIntersectAABB(const BWRay& ray, const Cuboid& AABB , float &t);
//bool RayIntersectOBB(const BWRay& ray , const Cuboid& OBB , float &t);
//���ߺ��������εĽ������  
//t:   ���ray����
//u,v: �������꣬�Ƕ�����p1p2 �� p1p3 
bool RayIntersectTriangle(const BWRay& ray, const BWPoint4DD& p1, const BWPoint4DD &p2, const BWPoint4DD& p3, float &t , float &u, float &v);
//Ҫ���ǵ�λ����,�� rayָ��normal ���򷵻�false
bool ReflcteRay(const BWVector3DD& normal, const BWRay& ray, BWPoint3DD& reflect);
//��������ϵת�����ѿ�������ϵ
BWPoint4DD SphereCoordToDescartesCoord(float theta, float phi, float radius);
//�������Բ�ֵ
BWPoint3DD LinearInterpolation(const BWPoint3DD& start, const BWPoint3DD& end , double k);
bool AABBIntersctRay(const BWRay& Ray, const BWVector3D &Min, const BWVector3D &Max);
//ע�� ��opengl��ʹ�þ������gl���ú���ʱ�������뷽ʽ��������� 
//�õ������վ��������ϵ��OpenGL������ʱת�õĹ�ϵ
void GetTranslateMatrix(double x, double y, double z, double * reslut);
void GetRotateMatrix(double theta, float x, float y, float z, double* reslut);
void GetScaleMatrix(float x, float y, float z , double * reslut);
//����������������һ������ϵת�����󣬸þ���Ϊ��������
void GetMatrixOfThreeVector(const BWPoint3DD& vec1, const BWPoint3DD& vec2, const BWPoint3DD& vec3 , double* reslut);
//��������ײ���
//bool CollisionDetecte(const Cuboid&, const Cuboid &);

//���� �Ÿ�� ����Գƾ��������ֵ����������
/*
  a:   n*n�ĶԳƾ���
  n:   ����ά��
  v:   ���յ���������
  eps: ���ȿ���
  jt:  ����������

  ���� -1 �����ڸõ��������£��޷���ɸþ��ȵļ���
  ���� 1  ������ȷ����
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

