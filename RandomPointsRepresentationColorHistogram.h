#pragma once
#include "colourHistogram.h"

class RandomPointsRepresentationColorHistogram : public colourHistogram
{

protected:
	Mat points;
	int windowSize;
	void rectifyPointsBounds(); // correct boundaries for each rectangle

public:
	RandomPointsRepresentationColorHistogram();
	RandomPointsRepresentationColorHistogram(Mat ImagetoBeRepresented, int numberOfBins, Mat XYPoints, int sizeOfWindow);
	~RandomPointsRepresentationColorHistogram();
	void computeRepresentation(Mat colourIndexArray, Mat LookUpTable, Mat initialMask) override;
	void computeRepresentation() override;
	float computeDistance(imageRepresentation*  targetRepresentation) override;
};

