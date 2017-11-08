#include "RandomPointsRepresentationColorHistogram.h"

RandomPointsRepresentationColorHistogram::RandomPointsRepresentationColorHistogram()
{
}

RandomPointsRepresentationColorHistogram::RandomPointsRepresentationColorHistogram(Mat ImagetoBeRepresented, int numberOfBins, Mat XYPoints, int sizeOfWindow)
{
	image = ImagetoBeRepresented;
	bins = numberOfBins;
	points = XYPoints;
	windowSize = sizeOfWindow;
	representationLength = pow(bins, image.channels()) * points.rows;
	representation = new float[representationLength];
}

RandomPointsRepresentationColorHistogram::~RandomPointsRepresentationColorHistogram()
{
}

void RandomPointsRepresentationColorHistogram::rectifyPointsBounds()
{
	// Keeps rectangular coordinates within image bounds
	float* p;
	for (int i = 0; i < points.rows; i++)
	{
		p = points.ptr<float>(i);
		int xtopLeft = p[0] - windowSize / 2;
		int ytopLeft = p[1] - windowSize / 2;
		if (p[0] < 0)
			p[0] = 0;
		if (p[0] >= image.cols - windowSize)
			p[0] = image.cols - windowSize - 1;
		if (p[1] < 0)
			p[1] = 0;
		if (p[1] >= image.rows - windowSize)
			p[1] = image.rows - windowSize - 1;
	}
}

void RandomPointsRepresentationColorHistogram::computeRepresentation(Mat colourIndexArray,  Mat LookUpTable, Mat initialMask)
{
	int ind = 0;
	float* p;

	for (int i = 0; i < points.rows; i++)
	{
		// For each point, crop an image rectangle and compute representation using colourHistogram Object

		p = points.ptr<float>(i);

		Rect r = Rect(p[0], p[1], windowSize, windowSize);
		Mat ROI = image(r);
		Mat ROIMask = initialMask(r);
		colourHistogram * colourHistogramRepresentation = new colourHistogram(ROI, bins);

		colourHistogramRepresentation->computeRepresentation(colourIndexArray, LookUpTable, ROIMask);

		int leng;
		const float * vect = colourHistogramRepresentation->getRepresentation(leng);

		// Concatenate representation vectors for each point
		for (int j = 0; j < leng; j++)
		{
			representation[ind] = vect[j];
			ind++;
		}

		delete colourHistogramRepresentation;
	}
}

void RandomPointsRepresentationColorHistogram::computeRepresentation()
{
	if ((image.empty()) || (image.depth() != CV_8U))
	{
		cout << "Empty/Invalid Image. Image should be of type CV_8U and within range 0-255 \n";
		representationLength = 0;
		return;
	}

	rectifyPointsBounds();
	Mat initialMask(image.rows, image.cols, CV_8UC1, cvScalar(1)); // Compute representation for all pixels
	Mat colourIndexArray = createColorIndexArray(pow(bins, image.channels()), image.channels());
	Mat LookUpTable = createBinaryLookupTable(bins);
	computeRepresentation(colourIndexArray, LookUpTable, initialMask);
}

float RandomPointsRepresentationColorHistogram::computeDistance(imageRepresentation*  targetRepresentationObj)
{
	int targetLength;
	const float * targetVector = targetRepresentationObj->getRepresentation(targetLength);
	
	if (representationLength != targetLength)
	{
		cout << "different lenghts \n";
		return FLT_MAX;
	}

	float * normalizedReferenceVector = normL1(representation, representationLength);
	float * normalizedTargetVector = normL1(targetVector, representationLength);

	int numPoints = points.rows;
	int localDescriptorLength = representationLength / numPoints;

	Mat metric2D(numPoints, numPoints, CV_32FC1);
	Mat indices2D(numPoints, numPoints, CV_32FC1);
	for (int i = 0; i < numPoints; i++)
	{
		// For each point in reference representation, find the closest point in traget representation and 
		// use its value as metric for that point

		float * metricArray = metric2D.ptr<float>(i);
		float * indexArray = indices2D.ptr<float>(i);
		float * currentRefPointer = normalizedReferenceVector + i * localDescriptorLength;
		vector<float> metricVector(numPoints);
		for (int j = 0; j < numPoints; j++)
		{
			float * currentTargetPointer = normalizedTargetVector + j * localDescriptorLength;
			metricVector[j] = histogramIntersectionMetric(currentRefPointer, localDescriptorLength, currentTargetPointer, localDescriptorLength);
			indexArray[j] = j;
		}
		std::sort(indexArray, indexArray + numPoints, [&metricVector](size_t i1, size_t i2) {return metricVector[i1] > metricVector[i2]; });
		for (int j = 0; j < numPoints; j++)
			metricArray[j] = metricVector[indexArray[j]];
	}

	Mat currentColumn = metric2D.col(0);

	delete[] normalizedReferenceVector;
	delete[] normalizedTargetVector;

	return 1 - cv::sum(currentColumn).val[0];

}