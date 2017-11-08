#pragma once
#include "imageRepresentation.h"

class colourHistogram :	public imageRepresentation
{

protected:

	int bins; // number of colour bins
	Mat createColorIndexArray(int vectorLength, int numChannels); // creates tuple of color indices
	Mat createBinaryLookupTable(int numBins); // creates binary lookup table for each bin

	float histogramIntersectionMetric(const float * referenceVector, const int referenceLength, const float * targetVector, const int targetLength);
	// returns histogram intersection metric (not normalized)

public:
	colourHistogram();
	colourHistogram(Mat ImagetoBeRepresented, int Mbins);
	~colourHistogram();

	virtual void computeRepresentation(Mat colourIndexArray, Mat LookUpTable, Mat initialMask);
	void computeRepresentation() override; // calls computeRepresentation defined above to compute representation vector
	float computeDistance(imageRepresentation*  targetRepresentation) override; // computes distance using histogramIntersectionMetric(...)
	
};

