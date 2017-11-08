#include "colourHistogram.h"

colourHistogram::colourHistogram()
{
}

colourHistogram::colourHistogram(Mat ImagetoBeRepresented, int Mbins)
{
	image = ImagetoBeRepresented;
	bins = Mbins;
	representationLength = pow(bins, image.channels());
	representation = new float[representationLength];
}

colourHistogram::~colourHistogram()
{
}

Mat colourHistogram::createColorIndexArray(int vectorLength, int numChannels)
{
	// this function creates color indices that look like something below for RGB channels
	// { 0    ,    0  ,    0;
	//   0    ,    0  ,    1;
	//   0    ,    0  ,    2;
	//   .    ,    .  ,    .
	//   .    ,    .  ,    .
    //   .    ,    .  ,    bins;
	//   0    ,    1  ,    0;
	//   0    ,    1  ,    1;
	//   .    ,    .  ,   .
	//   .    ,    .  ,   .
	//   bins , bins  , bins;}

	Mat colourIndexArray(vectorLength, numChannels, CV_8UC1);
	for (int i = 0; i < vectorLength; ++i)
	{
		int temp = i;
		for (int j = numChannels - 1; j > 0; --j)
		{
			colourIndexArray.at<uchar>(i, j) = temp % bins;
			temp = floor(temp / ((float)bins));
		}
		colourIndexArray.at<uchar>(i, 0) = temp;
	}
	return colourIndexArray;
}

Mat colourHistogram::createBinaryLookupTable(int numBins)
{
	// This function generates binary lookup table for each bin. It will be used to create a binary segmentation map for each color index
	// each color index as computed by above function createColorIndexArray(...)

	Mat LookUpTable(numBins, 256, CV_8U);
	for (int k = 0; k < numBins; ++k)
	{
		uchar* colorPointer = LookUpTable.ptr<uchar>(k);
		for (int colour = 0; colour < 256; ++colour)
		{
			if (floor(colour * (numBins / 256.0)) == k)
				colorPointer[colour] = 1;
			else
				colorPointer[colour] = 0;
		}
	}
	return LookUpTable;
}

void colourHistogram::computeRepresentation(Mat colourIndexArray, Mat LookUpTable, Mat initialMask)
{
	// initialMask determines whether or not compute representation for selected pixels

	int numChannels = image.channels();
	Mat* imgChannels = new Mat[numChannels];
	split(image, imgChannels);

	int channelLength = image.rows * image.cols;

	// create a binary mask for each pair of (numChannels,bins)
	int i, j, k;
	Mat** binaryMasks = new Mat*[numChannels];
	for (i = 0; i < numChannels; ++i)
	{
		binaryMasks[i] = new Mat[bins];
		for (k = 0; k < bins; ++k)
		{
			binaryMasks[i][k] = Mat(image.rows, image.cols, CV_8UC1);
			Mat KthTable(1, 256, CV_8U, LookUpTable.ptr<uchar>(k));
			LUT(imgChannels[i], KthTable, binaryMasks[i][k]);
		}
	}

	// Compute representation vector for each colour index based upon specifed mask and binary masks for each bin/channel
	//int s = 0;
	for (i = 0; i < representationLength; ++i)
	{	
		Mat sumMat(image.rows, image.cols, CV_8UC1, cvScalar(1));
		sumMat = sumMat & initialMask;
		for (j = 0; j < numChannels; ++j)
		{
			sumMat = sumMat & binaryMasks[j][colourIndexArray.at<uchar>(i, j)];
		}
		representation[i] = cv::sum(sumMat)[0];
		//s = s + representation[i];
		//cout << representation[i] << "\t";
	}
	//cout << "sum " << s << endl;

	// free memory resources
	for (i = 0; i < numChannels; ++i)
	{
		for (k = 0; k < bins; ++k)
		{
			binaryMasks[i][k].release();
		}
		delete[] binaryMasks[i];
		imgChannels[i].release();
	}

	delete[] binaryMasks;
	delete[] imgChannels;
}

void colourHistogram::computeRepresentation()
{
	if ((image.empty()) || (image.depth() != CV_8U))
	{
		cout << "Empty/Invalid Image. Image should be of type CV_8U and within range 0-255 \n";
		representationLength = 0;
		return;
	}

	Mat colourIndexArray = createColorIndexArray(representationLength, image.channels());
	Mat LookUpTable = createBinaryLookupTable(bins);
	Mat initialMask(image.rows, image.cols, CV_8UC1, cvScalar(1));

	computeRepresentation(colourIndexArray, LookUpTable, initialMask);
}

float colourHistogram::histogramIntersectionMetric(const float * referenceVector, const int referenceLength, const float * targetVector, const int targetLength)
{
	float metric = 0;
	for (int i = 0; i < referenceLength; i++)
		metric = metric + std::min(referenceVector[i], targetVector[i]);

	return metric;
}

float colourHistogram::computeDistance(imageRepresentation*  targetRepresentationObj)
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

	// histogram intersection
	float metric = histogramIntersectionMetric(normalizedReferenceVector, representationLength, targetVector, targetLength);
	metric = 1 - metric;

	delete[] normalizedReferenceVector;
	delete[] normalizedTargetVector;

	return metric;

}