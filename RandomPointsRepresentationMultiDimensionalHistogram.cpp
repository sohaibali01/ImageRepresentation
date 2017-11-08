#include "RandomPointsRepresentationMultiDimensionalHistogram.h"

RandomPointsRepresentationMultiDimensionalHistogram::RandomPointsRepresentationMultiDimensionalHistogram()
{
}

RandomPointsRepresentationMultiDimensionalHistogram::RandomPointsRepresentationMultiDimensionalHistogram(Mat ImagetoBeRepresented, Mat depthMap, int numberOfColourBins, int numberOfDepthBins, Mat XYPoints, int sizeOfWindow)
{
	image = ImagetoBeRepresented;
	depthImage = depthMap;
	bins = numberOfColourBins;
	depthBins = numberOfDepthBins;
	points = XYPoints;
	windowSize = sizeOfWindow;
	representationLength = pow(bins, image.channels()) * depthBins * points.rows;
	representation = new float[representationLength];
}

RandomPointsRepresentationMultiDimensionalHistogram::~RandomPointsRepresentationMultiDimensionalHistogram()
{
}

void RandomPointsRepresentationMultiDimensionalHistogram::computeRepresentation()
{
	if ((image.empty()) || (image.depth() != CV_8U) || (depthImage.empty()) || (depthImage.depth() != CV_8U))
	{
		cout << "Empty/Invalid Image. Image should be of type CV_8U and within range 0-255 \n";
		representationLength = 0;
		return;
	}

	if ((depthImage.rows != image.rows) || (depthImage.cols != image.cols))
	{
		cout << "Size of dpeth and colour image must match \n";
		representationLength = 0;
		return;
	}

	rectifyPointsBounds();
	int numChannels = image.channels();
	Mat colourIndexArray = createColorIndexArray(pow(bins, numChannels), numChannels);
	Mat LookUpTable = createBinaryLookupTable(bins);
	Mat depthLookUpTable = createBinaryLookupTable(depthBins);
	Mat* binaryDepthMasks = new Mat[depthBins];

	for (int i = 0; i < depthBins; ++i)
	{
		// create mask from depthlookuptable
		Mat ithDepthTable(1, 256, CV_8U, depthLookUpTable.ptr<uchar>(i));
		binaryDepthMasks[i] = Mat(image.rows, image.cols, CV_8UC1);
		LUT(depthImage, ithDepthTable, binaryDepthMasks[i]);
	}

	int ind = 0;
	float* p;
	for (int i = 0; i < points.rows; i++)
	{
		// For each pair of point, create an colourhistogram object, compute representations belonging to different depth masks
		// and concatenate resulting representations

		p = points.ptr<float>(i);

		Rect r = Rect(p[0], p[1], windowSize, windowSize);
		Mat ROI = image(r);
		colourHistogram * colourHistogramRepresentation = new colourHistogram(ROI, bins);

		for (int j = 0; j < depthBins; ++j)
		{
			Mat ROIMask = binaryDepthMasks[j](r);
			colourHistogramRepresentation->computeRepresentation(colourIndexArray, LookUpTable, ROIMask);

			int leng;
			const float * vect = colourHistogramRepresentation->getRepresentation(leng);

			for (int k = 0; k < leng; k++)
			{
				representation[ind] = vect[k];
				ind++;
			}
		}
		delete colourHistogramRepresentation;
	}

	for (int i = 0; i < depthBins; ++i)
		binaryDepthMasks[i].release();

	delete[] binaryDepthMasks;

}