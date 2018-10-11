#include "glew.h"
#include "glut.h"
#include <iostream>
#include "RealTimeRenderer/RealTimeRenderer.h"
#include "Scene/Camera.h"
#include "Test/TestFBXLoad.h"
#include "Test/TestFBXDrawObj.h"
#include "Scene/Object.h"
#include "Scene/Scene.h"

RealTimeRenderer<Object> RTRenderer;
Camera CameraForRender;
Scene Sceen;


TestFBXLoad FBXload;
//TestFBXDrawObj TestFBXObj;

bool IsContorllCamera = false;
int OldMouseX = 0;
int OldMouseY = 0;
void ProcessKeyboard(unsigned char key, int x, int y)
{
	BWVector3D Direction;
	float Speed = 0.5;

	static float a = 0;
	a += 0.01;
	BWVector3D Dir(0, 1.0, 0);
	Radian rad(a);
	switch (key)
	{
	case 'w':
	case 'W':
		Direction = CameraForRender.GetDirection();
		Direction.normalize();
		Direction = Direction * Speed;
		break;
	case 'S':
	case 's':
		Direction = CameraForRender.GetDirection();
		Direction.normalize();
		Direction = Direction *(-1) * Speed;
		break;
	case 'A':
	case 'a':
		Direction = CameraForRender.GetRight();
		Direction.normalize();
		Direction = Direction *(-1) * Speed;
		break;
	case 'D':
	case 'd':
		Direction = CameraForRender.GetRight();
		Direction.normalize();
		Direction = Direction * Speed;
		break;
	case 'i':
	case 'I':
		Sceen.GetObjectByName("elephant")->SetScale(a , a , a);
	default:
		break;
	}
	CameraForRender.Move(Direction);
	glutPostRedisplay();
}
void display()
{

	RTRenderer.BeginFrame();

	RTRenderer.SetLineWidth(1);
	RTRenderer.UseProgram();
	RTRenderer.SetLineColor(1.0, 0.0, 0.0);
	RTRenderer.SetViewMatrix(CameraForRender.GetViewMatrix());
	RTRenderer.SetProjectMatrix(CameraForRender.GetProjectMatrix());

	RTRenderer.ShowCoord();
	

	//BWRay Ray;
	//Ray._start = BWVector4DD(0, 0, 0, 0.0 );
	//Ray._vector = BWVector3D(100, 10, 0);

	//RTRenderer.SetLineWidth(1);
	//RTRenderer.SetLineColor(0.0, 1.0, 1.0);/**/
	//std::vector<BWVector3D> Lines;
	//Lines.push_back(BWVector3D(0.0, 0.0, 0.0));
	//Lines.push_back(BWVector3D(100.0, 10.0, 0.0));
	//RTRenderer.DrawLine(Lines);

	//IntersectionInfo Reslut;
	//if (Sceen.GetIntersectionInfo(Ray, Reslut))
	//{
	//	RTRenderer.SetLineWidth(5);
	//	RTRenderer.SetLineColor(0.0, 1.0, 0.0);
	//	std::vector<BWVector3D> Line;
	//	Line.push_back(Reslut.IntersetionTriangleInfo.P[0]);
	//	Line.push_back(Reslut.IntersetionTriangleInfo.P[1]);
	//	Line.push_back(Reslut.IntersetionTriangleInfo.P[2]);
	//	Line.push_back(Reslut.IntersetionTriangleInfo.P[0]);
	//	RTRenderer.DrawLine(Line);

	//	RTRenderer.SetLineColor(0.0, 0.0, 0.0);
	//	Line.clear();
	//	Line.push_back(Ray._start);
	//	Line.push_back(Reslut.IntersectionPoint);
	//	RTRenderer.DrawLine(Line);

	//	RTRenderer.SetLineWidth(1);
	//	RTRenderer.SetLineColor(1.0, 0.0, 0.0);/**/
	//}
	//RTRenderer.SetLineWidth(1);
	//RTRenderer.SetLineColor(1.0, 0.0, 0.0);/**/
	//Sceen.DrawScene();

	//Test Code Start
	//TestFBXObj.Draw();
    //Test Code End

	RTRenderer.Draw();

	RTRenderer.EndFrame();
}
void mouse(int x, int y)
{	
	if (IsContorllCamera && (OldMouseX != x || OldMouseY != y ))
	{
		float DeltaX = x - OldMouseX;
		float DeltaY = y - OldMouseY;
		DeltaX = DeltaX / 800 * 3.14;
		DeltaY = DeltaY / 800 * 3.14;
		CameraForRender.SetFixedYawAxis(true);
		CameraForRender.Yaw(Radian(DeltaX * (-1.0)));
		CameraForRender.Pitch(Radian(DeltaY * (-1.0)));
		OldMouseY = y;
		OldMouseX = x;
	}
	glutPostRedisplay();
}
void moueMoveFunction(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		IsContorllCamera = true;
		OldMouseX = x;
		OldMouseY = y;
	}
	else
	{
		IsContorllCamera = false;
	}
	//glutPostRedisplay();
}
int main(int argc, char **argv)
{
	int Width, Height;
	Width = 1600;
	Height = 800;
	
	
	

	glutInit(&argc, argv);
	glutInitWindowSize(Width, Height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("RayTracing");

	RTRenderer.Init(Width, Height);
	CameraForRender.Init();
	//Sceen.InitScene(&RTRenderer);
	Sceen.AddObject("elephant.obj", std::string("elephant"));
	RTRenderer.AddDrawable(Sceen.GetObjectByName("elephant"));

	//Test Code start
	//FBXload.ImportFBX("1M_Cube.FBX");
	//TestFBXObj.InitObj(&RTRenderer, &FBXload);
	//Test Code end

	glutKeyboardFunc(&ProcessKeyboard);
	glutMotionFunc(mouse);
	glutMouseFunc(moueMoveFunction);
	glutDisplayFunc(display);
	glutMainLoop();
	
	return 0;
}