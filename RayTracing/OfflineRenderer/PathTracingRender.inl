template<typename SceneType>
void PathTracingRender<SceneType>::RenderScene(SceneType* Scene)
{
	if (!Camera)  return;

	this->Scene = Scene;
	this->Scene->UpdateSceneInfo();
	RendererIntegrator->Init(this->Scene);
	RendererIntegrator->RequestSample(*OrigSample);
	ParallelProcess([&](std::vector<Task*> &Tasks)
	{
		const int SplitScreenWidth = 64;
		std::vector<Bounds2i> SplitScreenBounds;
		ScreenFilm.FilmBounds.SplitBounds(SplitScreenWidth, SplitScreenBounds);
		for (size_t i = 0; i < SplitScreenBounds.size(); i++)
		{
			Tasks.push_back(new PathTracingRenderTask<SceneType>(this, SplitScreenBounds[i]));
		}
	});
}


template<typename SceneType>
void PathTracingRenderTask<SceneType>::Run()
{
	SceneType *Scene = Render->GetScene();
	Film<typename SceneType::CameraType> *CameraFilm = Render->GetFilm();
	Sampler *MainSampelr = Render->GetMainSampler();
	Sampler *SubSampler = MainSampelr->GetSubSampler(ScreenBounds);
	int MaxSampleCount = SubSampler->GetMaxSampleCount();
	std::vector<Sample*> Samples = Render->GetOrigSample()->Duplicate(MaxSampleCount);
	::RNG Rng;
	int SampleNum = 0;
	while ((SampleNum = SubSampler->GetMoreSamples(Samples, &Rng)) != 0)
	{
		Spectrum Color(0.0f);
		for (int i = 0; i < SampleNum; i++)
		{
			IntersectionInfo Intersection;
			if (!CameraFilm->IsInTheFilm(Samples[i]->ImageX, Samples[i]->ImageY)) continue;
			BWRay Ray = CameraFilm->GetRayFromCamera(Samples[i]->ImageX, Samples[i]->ImageY);
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
					CoordinateSystem(Intersection.IntersectionNormal, &Intersection.IntersectionBiNormal, &Intersection.IntersectionTangent);
					Intersection.IntersectionBiNormal.normalize();
					Intersection.IntersectionTangent.normalize();
					Intersection.Material = Material;
				}
			};
			if (Scene->GetIntersectionInfo(Ray, GetIntersectionInfo))
			{
				Color += Render->GetIntegrator()->Li(Scene, &Intersection, *Samples[i], Rng);
			}
		}
		Color = Color / SampleNum;
		CameraFilm->SetSpectrum(Samples[0]->ImageX, Samples[0]->ImageY, &Color);
	}
	delete SubSampler;
	for (auto SampleData : Samples)
	{
		delete SampleData;
	}
}

template<typename SceneType>
PathTracingRenderTask<SceneType>::~PathTracingRenderTask()
{

}

template<typename SceneType>
PathTracingRenderTask<SceneType>::PathTracingRenderTask(PathTracingRender<SceneType> *Render, int StarPixelIndex, int EndPiexlIndex)
{
	this->Render = Render;
	this->StarPixelIndex = StarPixelIndex;
	this->EndPiexlIndex = EndPiexlIndex;
}
template<typename SceneType>
PathTracingRenderTask<SceneType>::PathTracingRenderTask(PathTracingRender<SceneType> *Render, const Bounds2i &InScreenBounds)
{
	this->Render = Render;
	ScreenBounds = InScreenBounds;
}