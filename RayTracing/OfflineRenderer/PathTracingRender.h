#pragma once


#include "..\V2\Filter.hpp"
#include "..\V2\Film.hpp"
#include "..\V2\Base\Textures.hpp"
#include "..\V2\Materials.hpp"
#include "..\V2\Util\Spectrum.hpp"
#include "..\V2\Lights.hpp"
class Sampler;
template<typename SceneType>
class PathTracingRender : public Renderer<SceneType>
{
public:
	typedef typename SceneType::CameraType CameraType;
	PathTracingRender(CameraType* Camera = nullptr, Sampler *MainSampler = nullptr) : Renderer<SceneType>(Camera, MainSampler) { }
	void TestV2()
	{

		BlackWalnut::FilterBase* Filter = new BlackWalnut::BoxFilter();

		float exposureTime = 1.0f;
		float ISO = 100.0f;
		float C = 314.159271;
		float fNumber = 1.0;
		float imagingRatio = BlackWalnut::Pi * exposureTime * ISO * BlackWalnut::K_m / (C * fNumber * fNumber);
		float whiteBalanceTemp = 1.0;
		BlackWalnut::PixelSensor* Sensor = new BlackWalnut::PixelSensor(BlackWalnut::RGBColorSpace::sRGB, whiteBalanceTemp, imagingRatio);
		BlackWalnut::FilmBaseParameters FileBaseParameter;
		FileBaseParameter.FileName = "Test.png";
		FileBaseParameter.FullResolution = BlackWalnut::Vector2i(1368, 1026);
		FileBaseParameter.Filter = Filter;
		FileBaseParameter.PixelBounds = Bounds2i(std::vector<int>{0, 0}, std::vector<int>{1368, 1026});
		FileBaseParameter.Sensor = Sensor;
		FileBaseParameter.Diagonal = 35.0f;
		BlackWalnut::FilmBase* Film = new BlackWalnut::RGBFilm(FileBaseParameter, BlackWalnut::RGBColorSpace::sRGB, FLT_MAX, true);
		BlackWalnut::SamplerBase* Sampler = new BlackWalnut::HaltonSampler(512, FileBaseParameter.FullResolution, 0);

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
			BlackWalnut::SpectrumTextureBase *Reflentance = new BlackWalnut::RGBReflectanceConstantTexture(*BlackWalnut::RGBColorSpace::sRGB, BlackWalnut::RGB(0.5, 0.5, 0.5));
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
			BlackWalnut::Matrix4X4f FinalTransform;
			Scale /= SpectrumToPhotometric(L);
			Light = new BlackWalnut::DistanceLight(FinalTransform, L, Scale);
		}
		
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
