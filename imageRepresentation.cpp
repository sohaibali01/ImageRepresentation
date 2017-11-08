#include "imageRepresentation.h"

imageRepresentation::imageRepresentation()
{
}

imageRepresentation::imageRepresentation(Mat ImagetoBeRepresented)
{
	image = ImagetoBeRepresented;
	representationLength = ImagetoBeRepresented.cols*ImagetoBeRepresented.rows*ImagetoBeRepresented.channels();
	representation = new float[representationLength];
	cout << "length " << representationLength << endl;
}

imageRepresentation::~imageRepresentation()
{
	delete[] representation;
}

const float* imageRepresentation::getRepresentation(int &outputLength) const
{
	const float* outputRepresentation = representation;
	outputLength = representationLength;

	return outputRepresentation;
}

void imageRepresentation::computeRepresentation()
{
	// Simply concatenate all pixels of an image to form a vector (perhaps the most naive representation)

	if ((image.empty()) || (image.depth() != CV_8U))
	{
		cout << "Empty/Invalid Image. Image should be of type CV_8U and within range 0-255 \n";
		representationLength = 0;
		return;
	}

	int channels = image.channels();
	int nRows = image.rows;
	int nCols = image.cols;

	int i;
	uchar* p = image.data;
	
	for (i = 0; i < representationLength; ++i)
		representation[i] = static_cast<float>(p[i]);

}

float * imageRepresentation::normL1(const float*  rep, int length)
{
	float * normalizedVector = new float[length];

	int s = 0;
	for (int i = 0; i < length; i++)
		s = s + rep[i];
	for (int i = 0; i < length; i++)
		normalizedVector[i] = rep[i] / s;

	return normalizedVector;
}

float imageRepresentation::computeDistance(imageRepresentation*  targetRepresentationObj)
{
	// simple L1 distance between two representations
	int targetLength;
	const float * targetVector = targetRepresentationObj->getRepresentation(targetLength);

	if (representationLength != targetLength)
	{
		cout << "different lenghts \n";
		return FLT_MAX;
	}

	float * normalizedReferenceVector = normL1(representation, representationLength);
	float * normalizedTargetVector = normL1(targetVector, representationLength);

	float metric = 0;
	int i;
	uchar* p = image.data;	
	for (i = 0; i < representationLength; ++i)
		metric = metric + abs(normalizedReferenceVector[i] - normalizedTargetVector[i]);

	return metric;
}
