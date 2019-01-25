#include "AppTest.h"
#include "TestFBXLoad.h"
#include "glew.h"
#include "glut.h"
#include "..\Scene\Light.h"
#include "..\OfflineRenderer\Sampler.h"
#include "TestMaterial.h"

//AUX_RGBImageRec * CreateTextureFromBmp()
//{
//	FILE *File = NULL;
//	const char *Filename = "Stucco2.bmp";
//	File = fopen(Filename, "r");
//	if (!File)
//		return 0;
//	fclose(File);
//	return auxDIBImageLoad(Filename);
//
//}

TestFBXLoad FBXload;
std::vector<BWVector3D> Lines;


void AppTest::Init(int Width, int Height)
{
	RTRenderer.Init(Width, Height);
	CameraForRender.Init(Width, Height);
	//Bottom
	{
		Spectrum BaseColor;
		Sceen.AddObject("cube.obj", std::string("obj1"));
		Object *TestObj = Sceen.GetObjectByName("obj1");
		TestObj->SetScale(15, 1, 10);
		
		BaseColor.SetValue(0, 196.0 / 255 * 0.7);
		BaseColor.SetValue(1, 165.0 / 255 * 0.7);
		BaseColor.SetValue(2, 97.0 / 255 * 0.7);

		TestObj->Material = new LambertianAndMicrofaceMateial(BaseColor);
		RTRenderer.AddDrawable(Sceen.GetObjectByName("obj1"));
	}
	//Back
	{
		Spectrum BaseColor;
		Sceen.AddObject("cube.obj", std::string("obj2"));
		Object *TestObj = Sceen.GetObjectByName("obj2");
		TestObj->SetPosition(0, 7.5, -5);
		TestObj->SetScale(15, 1, 15);
		TestObj->SetRoataion(BWVector3D(1.0, 0.0, 0.0), Radian(3.15 / 2));
		BaseColor.SetValue(0, 196.0 / 255 * 0.7);
		BaseColor.SetValue(1, 165.0 / 255 * 0.7);
		BaseColor.SetValue(2, 97.0 / 255 * 0.7);
		TestObj->Material = new LambertianAndMicrofaceMateial(BaseColor);
		RTRenderer.AddDrawable(Sceen.GetObjectByName("obj2"));
	}
	//Left
	{
		Spectrum BaseColor;
		Sceen.AddObject("cube.obj", std::string("obj3"));
		Object *TestObj = Sceen.GetObjectByName("obj3");
		TestObj->SetPosition(-7.5, 7.5, 0);
		TestObj->SetScale(15, 1, 10);
		TestObj->SetRoataion(BWVector3D(0.0, 0.0, 1.0), Radian(3.15 / 2));
		BaseColor.SetValue(0, 166.0 / 255 * 0.7);
		BaseColor.SetValue(1, 23.0 / 255 * 0.7);
		BaseColor.SetValue(2, 14.0 / 255 * 0.7);
		TestObj->Material = new LambertianAndMicrofaceMateial(BaseColor);
		RTRenderer.AddDrawable(Sceen.GetObjectByName("obj3"));
	}
	//Up
	{
		Spectrum BaseColor;
		Sceen.AddObject("cube.obj", std::string("obj4"));
		Object *TestObj = Sceen.GetObjectByName("obj4");
		TestObj->SetPosition(0, 15, 0);
		TestObj->SetScale(15, 1, 10);
		BaseColor.SetValue(0, 196.0 / 255 * 0.7);
		BaseColor.SetValue(1, 165.0 / 255 * 0.7);
		BaseColor.SetValue(2, 97.0 / 255 * 0.7);
		TestObj->Material = new LambertianAndMicrofaceMateial(BaseColor);
		RTRenderer.AddDrawable(Sceen.GetObjectByName("obj4"));
	}
	//Right
	{
		Spectrum BaseColor;
		Sceen.AddObject("cube.obj", std::string("obj5"));
		Object *TestObj = Sceen.GetObjectByName("obj5");
		TestObj->SetPosition(7.5, 7.5, 0);
		TestObj->SetScale(15, 1, 10);
		TestObj->SetRoataion(BWVector3D(0.0, 0.0, 1.0), Radian(3.15 / 2));
		BaseColor.SetValue(0, 63.0 / 255 * 0.7);
		BaseColor.SetValue(1, 124.0 / 255 * 0.7);
		BaseColor.SetValue(2, 181.0 / 255 * 0.7);
		TestObj->Material = new LambertianAndMicrofaceMateial(BaseColor);
		RTRenderer.AddDrawable(Sceen.GetObjectByName("obj5"));
	}

	{
		Spectrum BaseColor;
		BaseColor.SetValue(0, 32.0 / 255 * 0.7);
		BaseColor.SetValue(1, 73.0 / 255 * 0.7);
		BaseColor.SetValue(2, 15.0 / 255 * 0.7);
		Sceen.AddObject("cube.obj", std::string("obj6"));
		Object* TestObj = Sceen.GetObjectByName("obj6");
		TestObj->SetPosition(2, 3, 0);
		TestObj->SetScale(3, 6, 3);
		TestObj->SetRoataion(BWVector3D(0.0, 1.0, 0.0), Radian(3.15 / 4));
		TestObj->Material = new LambertianAndMicrofaceMateial(BaseColor);
		RTRenderer.AddDrawable(Sceen.GetObjectByName("obj6"));
	}
	{
		Spectrum BaseColor;
		BaseColor.SetValue(0, 32.0 / 255 * 0.7);
		BaseColor.SetValue(1, 73.0 / 255 * 0.7);
		BaseColor.SetValue(2, 15.0 / 255 * 0.7);
		Sceen.AddObject("cube.obj", std::string("obj7"));
		Object* TestObj = Sceen.GetObjectByName("obj7");
		TestObj->SetPosition(-3, 8, 0);
		TestObj->SetScale(2, 2, 2);
		TestObj->SetRoataion(BWVector3D(0.0, 1.0, 0.0), Radian(3.15 / 4));
		TestObj->SetRoataion(BWVector3D(1.0, 0.0, 0.0), Radian(3.15 / 4));
		TestObj->Material = new LambertianAndMicrofaceMateial(BaseColor);
		RTRenderer.AddDrawable(Sceen.GetObjectByName("obj7"));
	}
	
	{
		Spectrum BaseColor;
		BaseColor.SetValue(0, 196.0 / 255 * 0.7);
		BaseColor.SetValue(1, 165.0 / 255 * 0.7);
		BaseColor.SetValue(2, 97.0 / 255 * 0.7);
		Sceen.AddObject("planet.obj", std::string("obj8"));
		Object* TestObj = Sceen.GetObjectByName("obj8");
		TestObj->SetPosition(-3, 2, 0);
		TestObj->SetScale(1, 1, 1);
		TestObj->Material = new LambertianAndMicrofaceMateial(BaseColor);
		RTRenderer.AddDrawable(Sceen.GetObjectByName("obj8"));
	}

	/*DirectionLight *L = new DirectionLight();
	L->SetName(std::string("DirectionalLight"));
	L->SetPosition(BWVector3D(0.0, 10.0, 5.0));
	L->SetDirection(BWVector3D(0 , -5, 0));
	L->LightRadiance = Spectrum(1);
	Sceen.AddDirectionLight(L);*/

	{
		Object *Obj = new Object();
		Obj->LoadObjModel("cube.obj", "AreaLightObj");
		Obj->SetPosition(0, 10, 0);
		Obj->SetRoataion(BWVector3D(0.0, 1.0, 0.0), Radian(3.15 / 4));
		Obj->SetScale(2, 2, 2);
		RTRenderer.AddDrawable(Obj);

		Spectrum EmitColor;
		EmitColor.SetValue(0, 50.f);
		EmitColor.SetValue(1, 50.f);
		EmitColor.SetValue(2, 50.f);
		AreaLight *AreaL = new AreaLight();
		AreaL->SetName(std::string("AreaLight"));
		AreaL->SetEmitColor(EmitColor);
		AreaL->AddAreaLightShapeType(Obj);
		Sceen.AddAreaLight(AreaL);
	}
}

void AppTest::Update()
{
	if (!ShowOfflineRender)
	{
		UpdataSceneWithRealTimeRenderer();
	}
	else
	{
		UpdateSceneWithOfflineRenderer();
	}
}

void AppTest::ProcessKeyboard(unsigned char key, int x, int y)
{
	BWVector3D Direction;
	float Speed = 0.5;
	BWVector3D Dir(0, 1.0, 0);
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
	case 'C':
	case 'c':
	{
		ShowOfflineRender = !ShowOfflineRender;
		break;
	}
	default:
		break;
	}
	CameraForRender.Move(Direction);
	if (ShowOfflineRender)
	{
		DirectLightingIntegrator<Scene> LightingIntegrator;
		Random RandomSampler(0, CameraForRender.GetScreenWidth() * CameraForRender.GetScreenHeight(), 32, CameraForRender.GetScreenWidth(), CameraForRender.GetScreenHeight());
		OfflineRenderer.SetCamera(&CameraForRender);
		OfflineRenderer.SetIntegrator(&LightingIntegrator);
		OfflineRenderer.SetSampler(&RandomSampler);
		OfflineRenderer.RenderScene(&Sceen);
	}
	glutPostRedisplay();
}

void AppTest::MouseMove(int x, int y)
{
	if (IsContorllCamera && (OldMouseX != x || OldMouseY != y))
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

void AppTest::MouseStateChange(int button, int state, int x, int y)
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
}

void AppTest::UpdataSceneWithRealTimeRenderer()
{
	RTRenderer.BeginFrame();

	RTRenderer.SetLineWidth(1);
	RTRenderer.UseProgram();
	RTRenderer.SetLineColor(1.0, 0.0, 0.0);
	RTRenderer.SetViewMatrix(CameraForRender.GetViewMatrix());
	RTRenderer.SetProjectMatrix(CameraForRender.GetProjectMatrix());

	RTRenderer.ShowCoord();

	RTRenderer.SetLineColor(0.0, 0.0, 1.0);
	RTRenderer.DrawLine(Lines);
	RTRenderer.SetLineColor(1.0, 0.0, 0.0);

	RTRenderer.Draw();

	RTRenderer.EndFrame();
}

void AppTest::UpdateSceneWithOfflineRenderer()
{

	/*static unsigned char *Data = nullptr;
	int width = OfflineRenderer.GetFilm()->GetWidth();
	int height = OfflineRenderer.GetFilm()->GetHeight();
	if (!Data)
	{
		Data = new unsigned char[width*height * 3];
		for (int i = 0; i < height * width; i++)
		{
			Spectrum *Color = OfflineRenderer.GetFilm()->GetSpectrum(i);
			float RGB[3];
			Color->ToRGB(RGB);
			Data[i * 3] = RGB[0] * 255;
			Data[i * 3 + 1] = RGB[1] * 255;
			Data[i * 3 + 2] = RGB[2] * 255;
		}
	}

	RTRenderer.BeginFrame();
	RTRenderer.DrawImage(OfflineRenderer.GetFilm()->GetWidth(), OfflineRenderer.GetFilm()->GetHeight(), Data);
	RTRenderer.EndFrame();*/

	static float *Data = nullptr;
	int width = OfflineRenderer.GetFilm()->GetWidth();
	int height = OfflineRenderer.GetFilm()->GetHeight();
	if (!Data)
	{
		Data = new float[width*height * 3];
		for (int i = 0; i < height * width; i++)
		{
			Spectrum *Color = OfflineRenderer.GetFilm()->GetSpectrum(i);
			float RGB[3];
			Color->ToRGB(RGB);
			Data[i * 3] = RGB[0];
			Data[i * 3 + 1] = RGB[1];
			Data[i * 3 + 2] = RGB[2];
		}
	}

	RTRenderer.BeginFrame();
	RTRenderer.DrawImage(OfflineRenderer.GetFilm()->GetWidth(), OfflineRenderer.GetFilm()->GetHeight(), Data, GL_RGB32F, GL_RGB, GL_FLOAT);
	RTRenderer.EndFrame();
}

void AppTest::ShowAllNormal()
{
	std::vector<BWVector3D> Lines;
	TriangleInfo Info;
	for (int i = 0 ;i < Sceen.GetObjects().size(); i++ )
	{
		Object *Obj = Sceen.GetObjects()[i];
		for (int i = 0; i < Obj->GetTriangleNum(); i++)
		{
			Info.ClearData();
			Obj->GetTriangleInfoByIndex(i, Info);

			Lines.push_back(Info.P[0]);
			Lines.push_back(Info.P[0] + Info.N[0]);

			Lines.push_back(Info.P[1]);
			Lines.push_back(Info.P[1] + Info.N[1]);

			Lines.push_back(Info.P[2]);
			Lines.push_back(Info.P[2] + Info.N[2]);
		}
	}
	RTRenderer.SetLineColor(0.0, 0.0, 1.0);
	RTRenderer.DrawLine(Lines);
}

void AppTest::ShowAllAABB()
{
	std::vector<BWVector3D> Lines;
	for (int i = 0; i < Sceen.GetObjects().size(); i++)
	{
		Object *Obj = Sceen.GetObjects()[i];
		BWVector3D Min;
		BWVector3D Max;
		BWVector3D Ext;
		Obj->GetAABB(Min, Max);
		for (int i = 0; i < 3;i++)
		{
			Ext[i] = Max[i] - Min[i];
		}
		for (int i = 0; i < 3;i ++)
		{
			BWVector3D Temp = Min;
			Temp[i] += Ext[i];
			Lines.push_back(Min);
			Lines.push_back(Temp);
		}
		for (int i = 0; i < 3; i++)
		{
			BWVector3D Temp = Max;
			Temp[i] -= Ext[i];
			Lines.push_back(Max);
			Lines.push_back(Temp);
		}
	}
	RTRenderer.SetLineColor(0.0, 0.0, 1.0);
	RTRenderer.DrawLine(Lines);
}

