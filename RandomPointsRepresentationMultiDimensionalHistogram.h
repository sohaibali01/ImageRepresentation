#pragma once
#include "RandomPointsRepresentationColorHistogram.h"


class RandomPointsRepresentationMultiDimensionalHistogram : public RandomPointsRepresentationColorHistogram
{

private:
	Mat depthImage;
	int depthBins;

public:
	RandomPointsRepresentationMultiDimensionalHistogram();
	RandomPointsRepresentationMultiDimensionalHistogram(Mat ImagetoBeRepresented, Mat depthMap, int numberOfColourBins, int numberOfDepthBins, Mat XYPoints, int sizeOfWindow);
	~RandomPointsRepresentationMultiDimensionalHistogram();
	void computeRepresentation() override;
};

