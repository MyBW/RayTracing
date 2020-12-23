
//template<typename FilterType>
//BlackWalnut::FilterSampler::FilterSampler(FilterType Filter)
//{
//	Domain.Min = -Filter.Radius();
//	Domain.Max = Filter.Radius();
//	Values = Array2D<float>(Filter.Radius().X * 32, Filter.Radius().Y * 32);
//
//	for (int Y = 0; Y < Values.YSize(); ++Y)
//		for (int X = 0; X < Values.XSize(); ++X)
//		{
//			Vector2f P = Domain.Lerp(
//				Vector2f((X + 0.5f) / Values.XSize(), (Y + 0.5f) / Values.YSize()));
//			Values(X, Y) = Filter.Evaluate(P);
//		}
//
//	// Compute sampling distribution for filter
//	Distrib = PiecewiseConstant2D(Values, Domain);
//}