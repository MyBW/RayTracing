#include "AppTest.h"
#include "TestFBXLoad.h"
#include "glew.h"
#include "glut.h"
#include "..\Scene\Light.h"
#include "..\OfflineRenderer\Sampler.h"

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
	Sceen.AddObject("cube.obj", std::string("obj1"));
	Sceen.AddObject("planet.obj", std::string("obj2"));

	Object *TestObj = Sceen.GetObjectByName("obj1");
	TestObj->SetPosition(0, -1, -20);
	TestObj->SetScale(10, 1, 10);

	TestObj = Sceen.GetObjectByName("obj2");
	TestObj->SetPosition(0, 0, -20);
	TestObj->SetRoataion(BWVector3D(0.0, 1.0, 0.0), Radian(3.15 / 4));

	RTRenderer.AddDrawable(Sceen.GetObjectByName("obj1"));
	RTRenderer.AddDrawable(Sceen.GetObjectByName("obj2"));

	DirectionLight *L = new DirectionLight();
	L->SetName(std::string("DirectionalLight"));
	L->SetPosition(BWVector3D(0.0, 0.0, 5.0));
	Sceen.AddDirectionLight(L);

	//Test Code start
	//FBXload.ImportFBX("1M_Cube.FBX");
	//TestFBXObj.InitObj(&RTRenderer, &FBXload);
	//Test Code end

	// Test Offline Render
	//OfflineRenderer.SetCamera(&CameraForRender);
	//OfflineRenderer.RenderScene(&Sceen);


	/*int width = OfflineRenderer.GetFilm()->GetWidth();
	int height = OfflineRenderer.GetFilm()->GetHeight();
	Lines.clear();
	for (int i = 0; i < width * height; i += 10)
	{
		BWRay Ray = OfflineRenderer.GetFilm()->GetRayFromCamera(i);
		IntersectionInfo Info;
		if (Sceen.GetIntersectionInfo(Ray, Info))
		{
			Lines.push_back(Ray._start);
			Lines.push_back(Info.IntersectionPoint);
		}
	}*/
	
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
		DirectLightingIntegrator<Scene, IntersectionInfo> LightingIntegrator;
		OfflineRenderer.SetCamera(&CameraForRender);
		OfflineRenderer.SetIntegrator(&LightingIntegrator);

		Random RandomSampler(0, CameraForRender.GetScreenWidth() * CameraForRender.GetScreenHeight(), 1,CameraForRender.GetScreenWidth() , CameraForRender.GetScreenHeight());
		OfflineRenderer.SetSampler(&RandomSampler);
		OfflineRenderer.RenderScene(&Sceen);
		
		//int width = OfflineRenderer.GetFilm()->GetWidth();
		//int height = OfflineRenderer.GetFilm()->GetHeight();
		//Lines.clear();
		//for (int i = 0; i < width * height; i += 160)
		//{
		//	BWRay Ray = OfflineRenderer.GetFilm()->GetRayFromCamera(i);
		//	/*IntersectionInfo Info;
		//	if (Sceen.GetIntersectionInfo(Ray, Info))
		//	{
		//		Lines.push_back(Ray._start);
		//		Lines.push_back(Info.IntersectionPoint);
		//	}*/
		//}
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


	/*BWRay Ray;
	Lines.clear();
	Ray._start = BWVector4DD(0, 0, 0.0, 3.0 );
	Ray._vector = BWVector3D(0, 0.0, -1);


	Lines.push_back(Ray._start);
	Lines.push_back(Ray._start + Ray._vector * 10);
*/
	//ShowAllNormal();
	//RTRenderer.SetLineWidth(1);
	//RTRenderer.SetLineColor(1.0, 0.0, 0.0);/**/
	//Sceen.DrawScene();

	//Test Code Start
	//TestFBXObj.Draw();
	//Test Code End

	RTRenderer.SetLineColor(0.0, 0.0, 1.0);
	RTRenderer.DrawLine(Lines);
	RTRenderer.SetLineColor(1.0, 0.0, 0.0);

	RTRenderer.Draw();

	RTRenderer.EndFrame();
}

void AppTest::UpdateSceneWithOfflineRenderer()
{

	static unsigned char *Data = nullptr;
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

