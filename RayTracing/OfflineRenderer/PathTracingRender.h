#pragma once


#include "..\V2\Filter.hpp"
#include "..\V2\Film.hpp"
#include "..\V2\Base\Textures.hpp"
#include "..\V2\Materials.hpp"
#include "..\V2\Util\Spectrum.hpp"
#include "..\V2\Lights.hpp"
#include "..\V2\Integrators.hpp"
#include "..\V2\Shape\Shape.hpp"
class Sampler;
template<typename SceneType>
class PathTracingRender : public Renderer<SceneType>
{
public:
	typedef typename SceneType::CameraType CameraType;
	PathTracingRender(CameraType* Camera = nullptr, Sampler *MainSampler = nullptr) : Renderer<SceneType>(Camera, MainSampler) { }
	void TestV2(SceneType* Scene)
	{
		{
			/*using namespace BlackWalnut;
			BlackWalnut::Vector3f temp = { 0.0f, 0.0f, -1.0f };
			BlackWalnut::Frame TestFrame = BlackWalnut::Frame::FromZ(temp);
			BlackWalnut::Vector3f WorldPos = { 1.0f, 1.0f, 1.0f };
			BlackWalnut::Vector3f LocalPos = TestFrame.ToLocal(WorldPos);
			WorldPos = LocalPos;

			{
				BlackWalnut::Vector3f Pos = { 0.0f, 0.0f, 0.0f };
				BlackWalnut::Vector3f LookAt = {1.0, 1.0, 1.0};
				BlackWalnut::Vector3f Up = {};


				BlackWalnut::Vector3f X = { 1.0, 0, 0 };
				BlackWalnut::Vector3f Y = { 0.0, 1.0, 0 };

				TestFrame = BlackWalnut::Frame::FromXY(X, Y);
				BlackWalnut::Vector3f WorldPos = { 1.0f, 1.0f, 1.0f };
				BlackWalnut::Vector3f LocalPos = TestFrame.ToLocal(WorldPos);
				WorldPos = LocalPos;
			}*/
		}
		BlackWalnut::FilterBase* Filter = new BlackWalnut::BoxFilter();

		float exposureTime = 1.0f;
		float ISO = 100.0f;
		float C = 314.159271;
		float fNumber = 1.0;
		float imagingRatio = BlackWalnut::Pi * exposureTime * ISO * BlackWalnut::K_m / (C * fNumber * fNumber);
		float whiteBalanceTemp = 0.0;
		int SSP = 32;
		//BlackWalnut::Vector2i FullResolution = BlackWalnut::Vector2i(1368, 1026);
		//BlackWalnut::Vector2i FullResolution = BlackWalnut::Vector2i(2048, 1024);
		BlackWalnut::Vector2i FullResolution = BlackWalnut::Vector2i(512, 256);
		BlackWalnut::PixelSensor* Sensor = new BlackWalnut::PixelSensor(BlackWalnut::RGBColorSpace::sRGB, whiteBalanceTemp, imagingRatio);
		BlackWalnut::FilmBaseParameters FileBaseParameter;
		FileBaseParameter.FileName = "Test.png";
		FileBaseParameter.FullResolution = FullResolution;
		FileBaseParameter.Filter = Filter;
		FileBaseParameter.PixelBounds = Bounds2i(std::vector<int>{0, 0}, std::vector<int>{FullResolution.X, FullResolution.Y});
		FileBaseParameter.Sensor = Sensor;
		FileBaseParameter.Diagonal = 35.0f;
		BlackWalnut::FilmBase* Film = new BlackWalnut::RGBFilm(FileBaseParameter, BlackWalnut::RGBColorSpace::sRGB, FLT_MAX, true);
		BlackWalnut::SamplerBase* Sampler = new BlackWalnut::HaltonSampler(SSP, FileBaseParameter.FullResolution, 0);

		std::string TextureName = "lines.png";
		std::string FilterName = "bilinear";
		BlackWalnut::TextureMapping2D *Mapping = new BlackWalnut::UVMapping2D();
		BlackWalnut::WrapMode Mode = BlackWalnut::WrapMode::Repeat;
		BlackWalnut::SpectrumTextureBase *Texutre = new BlackWalnut::SpectrumImageTexture(Mapping, TextureName, FilterName, 8.0f, Mode, 1.0, BlackWalnut::ColorEncodingBase::sRGB);

		BlackWalnut::DiffuseMaterial *Diffuse1;
		{
			BlackWalnut::SpectrumTextureBase *Reflentance = new BlackWalnut::SpectrumConstantTexture(new BlackWalnut::ConstantSpectrum(0.5f));
			BlackWalnut::FloatConstantTexture *Sigma = new BlackWalnut::FloatConstantTexture(0.0f);
			Diffuse1 = new BlackWalnut::DiffuseMaterial(Reflentance, Sigma, nullptr);
		}
		

		BlackWalnut::DiffuseMaterial *Diffuse2;
		{
			BlackWalnut::SpectrumTextureBase *Reflentance = new BlackWalnut::RGBReflectanceConstantTexture(*BlackWalnut::RGBColorSpace::sRGB, BlackWalnut::RGB(0.9, 0.0, 0.0));
			BlackWalnut::FloatConstantTexture *Sigma = new BlackWalnut::FloatConstantTexture(0.0f);
			Diffuse2 = new BlackWalnut::DiffuseMaterial(Reflentance, Sigma, nullptr);
		}
		BlackWalnut::DiffuseMaterial *Diffuse3;
		{
			BlackWalnut::SpectrumTextureBase *Reflentance = Texutre;
			BlackWalnut::FloatConstantTexture *Sigma = new BlackWalnut::FloatConstantTexture(0.0f);
			Diffuse3 = new BlackWalnut::DiffuseMaterial(Reflentance, Sigma, nullptr);
		}
		BlackWalnut::ConductorMaterial *Conductor;
		{
			std::string KSpdFileName ="spd/Au.k.spd";
			std::string EtaSpdFileName = "spd/Au.eta.spd";
			BlackWalnut::BaseSpectrum *K = BlackWalnut::PiecewiseLinearSpectrum::Read(KSpdFileName);
			BlackWalnut::BaseSpectrum *Eta = BlackWalnut::PiecewiseLinearSpectrum::Read(EtaSpdFileName);
			BlackWalnut::SpectrumTextureBase* EtaTexture = new BlackWalnut::SpectrumConstantTexture(Eta);
			BlackWalnut::SpectrumTextureBase* KTexture = new BlackWalnut::SpectrumConstantTexture(K);
			BlackWalnut::FloatConstantTexture* URoughness = new BlackWalnut::FloatConstantTexture(0.002);
			BlackWalnut::FloatConstantTexture* VRoughness = new BlackWalnut::FloatConstantTexture(0.002);
			Conductor = new BlackWalnut::ConductorMaterial(EtaTexture, KTexture, URoughness, VRoughness, nullptr, true);
		}
		std::vector<BlackWalnut::LightBase*> Lights;
		BlackWalnut::DistanceLight* Light;
		{
			BlackWalnut::RGBSpectrum* L = new BlackWalnut::RGBSpectrum(*BlackWalnut::RGBColorSpace::sRGB, BlackWalnut::RGB(0.2f, 0.2f, 0.2f));
			float Scale = 1.0f;
			BlackWalnut::Vector3f From, To, W;
			From.MakeZero(); To.MakeZero();
			W = From - To;
			BlackWalnut::Vector3f V1, V2;
			//CoordinateSystem(W, &V1, &V2);
			/*float m[4][4] = { V1.X, V2.X, W.X, 0, V1.Y, V2.Y, W.Y, 0,
				V1.Z, V2.Z, W.Z, 0, 0,    0,    0,   1 };
			BlackWalnut::Matrix4X4f FinalTransform(m);*/

			BlackWalnut::Vector3f Pos = BlackWalnut::Vector3f(3.0f, 10.0f, 3.0f);
			BlackWalnut::Vector3f LookAt = BlackWalnut::Vector3f(0.0f);
			BlackWalnut::Vector3f Up = BlackWalnut::Vector3f(0.0, 1.0, 0.0);
			BlackWalnut::Matrix4X4f WorldToLight;
			BlackWalnut::Matrix4X4f LightToWorld;
			BlackWalnut::MakeViewMatrix(WorldToLight, Pos, LookAt, Up);
			BlackWalnut::InverseMatrix(LightToWorld, WorldToLight);
			Scale /= SpectrumToPhotometric(L);
			Light = new BlackWalnut::DistanceLight(LightToWorld, L, Scale);
			Light->SetSceneRadius(0.f);// 这个值会被用来计算光源和物体间的遮挡 对于该计算来说还不能正确计算
			//Lights.push_back(Light);
		}
		{
	
			BlackWalnut::Vector3f Pos = BlackWalnut::Vector3f(6.0f, 10.0f, 0.0f);
			BlackWalnut::Vector3f LookAt = BlackWalnut::Vector3f(0.0f);
			BlackWalnut::Vector3f Up = BlackWalnut::Vector3f(0.0, 0.0, -1.0);
			BlackWalnut::Matrix4X4f WorldToLight;
			BlackWalnut::Matrix4X4f LightToWorld;
			BlackWalnut::MakeViewMatrix(WorldToLight, Pos, LookAt, Up);
			BlackWalnut::InverseMatrix(LightToWorld, WorldToLight);

			BlackWalnut::ShapeBase* DiskShape = new BlackWalnut::DiskShape(LightToWorld, WorldToLight, false, 0, 1.5, 0, 360);
			BlackWalnut::RGBSpectrum* L = new BlackWalnut::RGBSpectrum(*BlackWalnut::RGBColorSpace::sRGB, BlackWalnut::RGB(50.f, 50.f, 50.f));
			float Scale = 1.0f;
			Scale /= SpectrumToPhotometric(L);
			BlackWalnut::DiffuseAreaLight* AreaLight = new BlackWalnut::DiffuseAreaLight(LightToWorld, L, Scale, DiskShape, nullptr, false);
			Lights.push_back(AreaLight);
		}
		int32_t MaxDepth = 5;
		bool IsSampleLights = true;
		bool IsSampleBSDF = true;
		BlackWalnut::SimplePathIntegrator * PathIntegrator = nullptr; 
		BlackWalnut::CameraBase *BKCamera = nullptr;
		{
			BlackWalnut::CameraBaseParameters Param;
			float Frame = Film->GetFullResolution().X / Film->GetFullResolution().Y;
			Param.CameraTransform.MakeZero();
			Param.Film = Film;
			Param.ShutterClose = 1.0f;
			Param.ShutterOpen = 0.0f;
			BlackWalnut::Matrix4X4f ScreenFromCamera;
			Bounds2f Screen;
			Screen.Min[0] = -Frame;
			Screen.Max[0] = Frame;
			Screen.Min[1] = -1.0;
			Screen.Max[1] = 1.0;
			
			BlackWalnut::ProjectiveCamera<CameraType> *ProjCamera =  new BlackWalnut::ProjectiveCamera<CameraType>(Param, ScreenFromCamera, Screen, 0.0f, 1000000.0f);
			ProjCamera->OutCamera = Camera;
			ProjCamera->UpdateDxDy();
			BKCamera = ProjCamera;


			PathIntegrator  = new BlackWalnut::SimplePathIntegrator(MaxDepth, IsSampleLights, IsSampleBSDF, BKCamera, Sampler, Lights);
			{
				
				auto V2GetIntersectionInfo = [&](const BlackWalnut::Ray &InRay, float TMax)->BlackWalnut::ShapeIntersection
				{
					BWRay ray;
					ray._start.x = InRay.O.X;
					ray._start.y = InRay.O.Y;
					ray._start.z = InRay.O.Z;
					ray._vector.x = InRay.D.X;
					ray._vector.y = InRay.D.Y;
					ray._vector.z = InRay.D.Z;

					IntersectionInfo Intersection;
					float Mint = FLT_MAX;
					auto GetIntersectionInfo = [&Intersection, &Mint](const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &Ray, const RTMaterial* Material, bool &IsBreak)
					{
						if (t < Mint)
						{
							Mint = t;
							Intersection.IntersectionPoint = Ray._start + Ray._vector * t;
							Intersection.InputRay = -Ray;
							Intersection.TriangleP = P;
							Intersection.TriangleN = N;
							Intersection.TriangleUV = UV;
							Intersection.IntersectionNormal = LinearInterpolation(Intersection.TriangleN[0], Intersection.TriangleN[1], u);
							Intersection.IntersectionNormal = LinearInterpolation(Intersection.IntersectionNormal, Intersection.TriangleN[2], v);
							Intersection.IntersectionNormal.normalize();
							Intersection.IntersectionUV = LinearInterpolation(Intersection.TriangleUV[0], Intersection.TriangleUV[1], u);
							Intersection.IntersectionUV = LinearInterpolation(Intersection.IntersectionUV, Intersection.TriangleUV[2], v);
							CoordinateSystem(Intersection.IntersectionNormal, &Intersection.IntersectionBiNormal, &Intersection.IntersectionTangent);
							Intersection.IntersectionBiNormal.normalize();
							Intersection.IntersectionTangent.normalize();
							Intersection.Material = Material;
						}
					};
					if (Scene->GetIntersectionInfo(ray, GetIntersectionInfo))
					{
						auto CoverVector3 = [](BWVector3DD& Data)
						{
							BlackWalnut::Vector3f Vec;
							Vec.X = Data.x;
							Vec.Y = Data.y;
							Vec.Z = Data.z;
							return Vec;
						};
						auto FromVector3 = [](BlackWalnut::Vector3f Data)
						{
							BWVector3DD Vec;
							Vec.x = Data.X;
							Vec.y = Data.Y;
							Vec.z = Data.Z;
							return Vec;
						};
						auto CoverVector2 = [](const BWPoint2DD& Data)
						{
							BlackWalnut::Vector2f Vec;
							Vec.X = Data.x;
							Vec.Y = Data.y;
							return Vec;
						};
						//World Space
						BlackWalnut::Vector3f p0 = CoverVector3(Intersection.TriangleP[0]);
						BlackWalnut::Vector3f p1 = CoverVector3(Intersection.TriangleP[1]);
						BlackWalnut::Vector3f p2 = CoverVector3(Intersection.TriangleP[2]);
						BlackWalnut::Vector3f n0 = CoverVector3(Intersection.TriangleN[0]);
						BlackWalnut::Vector3f n1 = CoverVector3(Intersection.TriangleN[1]);
						BlackWalnut::Vector3f n2 = CoverVector3(Intersection.TriangleN[2]);
						BlackWalnut::Vector2f uv0 = CoverVector2(Intersection.TriangleUV[0]);
						BlackWalnut::Vector2f uv1 = CoverVector2(Intersection.TriangleUV[1]);
						BlackWalnut::Vector2f uv2 = CoverVector2(Intersection.TriangleUV[2]);

						BlackWalnut::Vector2f duv02 = uv0 - uv2;
						BlackWalnut::Vector2f duv12 = uv0 - uv1;
						BlackWalnut::Vector3f dp02 = p0 - p2;
						BlackWalnut::Vector3f dp12 = p0 - p1;
						

						float determinant = BlackWalnut::DifferenceOfProducts(duv02.X, duv12.Y, duv02.Y, duv12.X);

						BlackWalnut::Vector3f dpdu, dpdv;
						bool degenerateUV = std::abs(determinant) < 1e-9f;
						if (!degenerateUV)
						{
							float invDet = 1 / determinant;
							dpdu = BlackWalnut::DifferenceOfProducts(duv12.Y, dp02, duv02.Y, dp12) * invDet;
							dpdv = BlackWalnut::DifferenceOfProducts(duv02.X, dp12, duv12.X, dp02) * invDet;
						}
						BlackWalnut::Vector3f Ret;
						BlackWalnut::CrossProduct(Ret, dpdu, dpdv);
						if (degenerateUV || BlackWalnut::Length(Ret) == 0)
						{
							BlackWalnut::Vector3f ng;
							BlackWalnut::CrossProduct(ng, p2 - p0, p1 - p0);
							if (BlackWalnut::Length(ng) == 0)
							{
								CHECK(0);
							}
							BWVector3DD V1, V2, V3;
							BlackWalnut::Normalize(ng);
							V1 = FromVector3(ng);
							CoordinateSystem(V1, &V2, &V3);
							dpdu = CoverVector3(V2);
							dpdv = CoverVector3(V3);
						}

						/*float xAbsSum =
							(std::abs(ti.b0 * p0.x) + std::abs(ti.b1 * p1.x) + std::abs(ti.b2 * p2.x));
						float yAbsSum =
							(std::abs(ti.b0 * p0.y) + std::abs(ti.b1 * p1.y) + std::abs(ti.b2 * p2.y));
						float zAbsSum =
							(std::abs(ti.b0 * p0.z) + std::abs(ti.b1 * p1.z) + std::abs(ti.b2 * p2.z));
						BlackWalnut::Vector3f pError = gamma(7) * BlackWalnut::Vector3f(xAbsSum, yAbsSum, zAbsSum);*/
						float time = 0;// for test
						BlackWalnut::SurfaceInteraction isect(CoverVector3(Intersection.IntersectionPoint), CoverVector2(Intersection.IntersectionUV),
							CoverVector3(Intersection.InputRay._vector), dpdu, dpdv, BlackWalnut::Vector3f(0.0f), BlackWalnut::Vector3f(0.0f), time);
						BlackWalnut::CrossProduct(isect.shading.n, dp02, dp12);
						BlackWalnut::Normalize(isect.shading.n);
						isect.n = isect.shading.n;
						{
							BlackWalnut::Vector3f n = CoverVector3(Intersection.IntersectionNormal);
							BlackWalnut::Vector3f nb = CoverVector3(Intersection.IntersectionBiNormal);
							BlackWalnut::Vector3f nt = CoverVector3(Intersection.IntersectionTangent);
							BlackWalnut::Vector3f dndu, dndv;
							{
								using namespace BlackWalnut;
								Vector2f duv02 = uv0 - uv2;
								Vector2f duv12 = uv1 - uv2;
								Vector3f dn1 = n0 - n2;
								Vector3f dn2 = n1 - n2;

								float determinant =
									DifferenceOfProducts(duv02.X, duv12.Y, duv02.Y, duv12.X);
								bool degenerateUV = std::abs(determinant) < 1e-9;
								if (degenerateUV) {
									// We can still compute dndu and dndv, with respect to the
									// same arbitrary coordinate system we use to compute dpdu
									// and dpdv when this happens. It's important to do this
									// (rather than giving up) so that ray differentials for
									// rays reflected from triangles with degenerate
									// parameterizations are still reasonable.

									Vector3f dn;
									CrossProduct(dn, n2 - n0, n1 - n0);
									Normalize(dn);
									if (Length(dn) == 0)
										dndu = dndv = Vector3f(0, 0, 0);
									else {

										BWVector3DD V1, V2, V3;
										V1 = FromVector3(dn);
										::CoordinateSystem(V1, &V2, &V3);
										dndu = CoverVector3(V2);
										dndv = CoverVector3(V3);
									}
								}
								else {
									float invDet = 1 / determinant;
									dndu = DifferenceOfProducts(duv12.Y, dn1, duv02.Y, dn2) * invDet;
									dndv = DifferenceOfProducts(duv02.X, dn2, duv12.X, dn1) * invDet;
								}
							}
							isect.SetShadingGeometry(n, nb, nt, dndu, dndv, true);
						}

						BlackWalnut::ShapeIntersection Info;
						Info.THit = Mint;
						Info.Intr = isect;
						Info.Intr.material = Diffuse1;

						BSDF Bsdf;
						Intersection.Material->CreateBSDF(Intersection, Bsdf);
						if (Bsdf.Indxe == 1)
						{
							
							Info.Intr.material = Conductor;
						}
						if (Bsdf.Indxe == 2)
						{
							Info.Intr.material = Diffuse2;
						}

						//Info.Intr.AreaLightSrc = nullptr;
						//Info.Intr.dndu = Info.Intr.dndv = BlackWalnut::Vector3f(0.0, 0.0, 0.0);
						//Info.Intr.dpdu = Info.Intr.dpdv = BlackWalnut::Vector3f(0.0, 0.0, 0.0);
						//Info.Intr.dpdx = Info.Intr.dpdy = BlackWalnut::Vector3f(0.0, 0.0, 0.0);
						//Info.Intr.dudx = Info.Intr.dudy = 0.0f;
						//Info.Intr.dvdx = Info.Intr.dvdy = 0.0f;
						//Info.Intr.faceIndex = 0;
						//Info.Intr.n = CoverVector(Intersection.IntersectionNormal);
						//Info.Intr.material = Diffuse1;
						//Info.Intr.pi = CoverVector(Intersection.IntersectionPoint);
						//Info.Intr.shading.dndu = Info.Intr.shading.dndv = Info.Intr.shading.dpdu = Info.Intr.shading.dpdv = BlackWalnut::Vector3f(0.0, 0.0, 0.0);
						//Info.Intr.shading.n = CoverVector(Intersection.IntersectionNormal);
						//Info.Intr.time = 0;
						//
						////Info.Intr.uv = Intersection.TriangleUV;
						//Info.Intr.wo = CoverVector(Intersection.InputRay._vector);
						return Info;
					}
					return{};
				};
				PathIntegrator->IntersectFun = V2GetIntersectionInfo;
				auto V2GetIntersectionInfoP = [&](const BlackWalnut::Ray &InRay, float TMax)
				{
					BWRay ray;
					ray._start.x = InRay.O.X;
					ray._start.y = InRay.O.Y;
					ray._start.z = InRay.O.Z;
					BlackWalnut::Vector3f D = InRay.D;
					float Length =  BlackWalnut::Length(InRay.D) ;
					Normalize(D);
					ray._vector.x = D.X;
					ray._vector.y = D.Y;
					ray._vector.z = D.Z;

					IntersectionInfo Intersection;
					float Mint = FLT_MAX;
					auto GetIntersectionInfo = [&Intersection, &Mint](const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &Ray, const RTMaterial* Material, bool &IsBreak)
					{
						if (t < Mint)
						{
							Mint = t;
							Intersection.IntersectionPoint = Ray._start + Ray._vector * t;
							Intersection.InputRay = -Ray;
							Intersection.TriangleP = P;
							Intersection.TriangleN = N;
							Intersection.TriangleUV = UV;
							Intersection.IntersectionNormal = LinearInterpolation(Intersection.TriangleN[0], Intersection.TriangleN[1], u);
							Intersection.IntersectionNormal = LinearInterpolation(Intersection.IntersectionNormal, Intersection.TriangleN[2], v);
							Intersection.IntersectionNormal.normalize();
							Intersection.IntersectionUV = LinearInterpolation(Intersection.TriangleUV[0], Intersection.TriangleUV[1], u);
							Intersection.IntersectionUV = LinearInterpolation(Intersection.IntersectionUV, Intersection.TriangleUV[2], v);
							CoordinateSystem(Intersection.IntersectionNormal, &Intersection.IntersectionBiNormal, &Intersection.IntersectionTangent);
							Intersection.IntersectionBiNormal.normalize();
							Intersection.IntersectionTangent.normalize();
							Intersection.Material = Material;
						}
					};
					if (Scene->GetIntersectionInfo(ray, GetIntersectionInfo))
					{
						float Distance = Lenth(ray._vector * Mint);
						return Distance < Length;
					}
					return false;
				};
				PathIntegrator->IntersectPFun = V2GetIntersectionInfoP;
			}
		}
		

		PathIntegrator->Render();
	}
	void RenderScene(SceneType* Scene) override;
};



template<typename SceneType>
class PathTracingRenderTask :public Task
{
public:
	PathTracingRenderTask(PathTracingRender<SceneType> *Render, int StarPixelIndex, int EndPiexlIndex);
	PathTracingRenderTask(PathTracingRender<SceneType> *Render, const Bounds2i &InScreenBounds);
	~PathTracingRenderTask();
	void Run() override;
private:
	PathTracingRender<SceneType> *Render;
	Bounds2i ScreenBounds;
	int StarPixelIndex;
	int EndPiexlIndex;
};



#include "PathTracingRender.inl"
