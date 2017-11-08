
#include "RandomPointsRepresentationMultiDimensionalHistogram.h"

#include <opencv2/opencv.hpp>
#include "opencv2/opencv_modules.hpp"
# include "opencv2/core/core.hpp"
# include "opencv2/features2d/features2d.hpp"
# include "opencv2/highgui/highgui.hpp"
#include "opencv2/xfeatures2d.hpp"

Mat generateSiftKeypoints(Mat img, int T)
{
	Mat pts(T, 2, CV_32FC1, 0.0);
	float* p;
	int minHessian = 400;
	cv::Ptr<cv::Feature2D> features = cv::xfeatures2d::SURF::create(minHessian);
	std::vector<cv::KeyPoint> keypoints;
	features->detect(img, keypoints);
	if (keypoints.size() < T)
		cout << "Warning....... not enough keypoints detected \n";

	std::sort(keypoints.begin(), keypoints.end(), [](cv::KeyPoint a, cv::KeyPoint b) { return a.response > b.response; });
	for (int i = 0; i < T; i++)
	{
		p = pts.ptr<float>(i);
		p[0] = keypoints[i].pt.x;
		p[1] = keypoints[i].pt.y;

		//cout << pts.at<float>(i, 0) << "\t";
		//cout << pts.at<float>(i, 1) << "\n";
	}
	return pts;
}

Mat generateRandomPointsFromImage(Mat img, int T)
{
	// returns a Mat of points. 1st row contains x chich changes horizontally. 2nd row contains y coordinates which change vertically

	Mat pts(T, 2, CV_32FC1, 0.0);
	float* p;
	RNG rng(getCPUTickCount());
	for (int i = 0; i < T; i++)
	{
		p = pts.ptr<float>(i);
		
		int y = rng.uniform(0, img.rows);
		int x = rng.uniform(0, img.cols);

		p[0] = x;
		p[1] = y;

		//cout << pts.at<float>(i, 0) << "\t";
		//cout << pts.at<float>(i, 1) << "\n";
	}
	return pts;
}

int main(int argc, char **argv)
{

	if (argc != 9)
	{
		cout << "wrong number of arguments. Enter dummy values if you like to ignore any argument \n";
			return 0;
	}

	Mat referenceColourImage = imread(string(argv[1]), CV_LOAD_IMAGE_COLOR);
	Mat targetColourImage = imread(string(argv[2]), CV_LOAD_IMAGE_COLOR);
	Mat referenceDepthImage = imread(string(argv[3]), CV_LOAD_IMAGE_GRAYSCALE);
	Mat targetDepthImage = imread(string(argv[4]), CV_LOAD_IMAGE_GRAYSCALE);
	int numberColourBins = stoi(string(argv[6]));
	int numberDepthBins = stoi(string(argv[6]));
	int numberPoints = stoi(string(argv[7]));
	int windowSize = stoi(string(argv[8]));
	string representationName(argv[5]);

	//Mat referenceColourImage = imread("sohaib.jpg", CV_LOAD_IMAGE_COLOR);
	//Mat referenceDepthImage = imread("sohaib.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//Mat targetColourImage = imread("sohaibFilteredResized.jpg", CV_LOAD_IMAGE_COLOR);
	//Mat targetDepthImage = imread("sohaibFilteredResized.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//int numberColourBins = 5;
	//int numberDepthBins = 5;
	//int numberPoints = 200;
	//int windowSize = 50;
	//string representationName("randomCH");

	Mat referencePoints, targetPoints;
	if ((representationName == "randomCH") || (representationName == "randomMDH"))
	{
		int minDimension = min(min(referenceColourImage.rows, referenceColourImage.cols), min(targetColourImage.rows, targetColourImage.cols));
		if (minDimension < windowSize + 2)
		{
			cout << "image size too small compared to window size specified.... \n";
			return 0;
		}

		referencePoints = generateSiftKeypoints(referenceColourImage, numberPoints);
		targetPoints = generateSiftKeypoints(targetColourImage, numberPoints);
		//referencePoints = generateRandomPointsFromImage(referenceColourImage, numberPoints);
		//targetPoints = generateRandomPointsFromImage(targetColourImage, numberPoints);

	}

	imageRepresentation * referenceColourRepresentationObject;
	imageRepresentation * targetColourRepresentationObject;


	if (representationName == "CH")
	{
		referenceColourRepresentationObject = new colourHistogram(referenceColourImage, numberColourBins);
		targetColourRepresentationObject = new colourHistogram(targetColourImage, numberColourBins);
	}
	
	else if (representationName == "randomCH")
	{
		referenceColourRepresentationObject = new RandomPointsRepresentationColorHistogram(referenceColourImage, numberColourBins, referencePoints, windowSize);
		targetColourRepresentationObject = new RandomPointsRepresentationColorHistogram(targetColourImage, numberColourBins, targetPoints, windowSize);
	}

	else if (representationName == "randomMDH")
	{
		referenceColourRepresentationObject = new RandomPointsRepresentationMultiDimensionalHistogram(referenceColourImage, referenceDepthImage, numberColourBins, numberDepthBins, referencePoints, windowSize);
		targetColourRepresentationObject = new RandomPointsRepresentationMultiDimensionalHistogram(targetColourImage, targetDepthImage, numberColourBins, numberDepthBins, targetPoints, windowSize);
	}

	referenceColourRepresentationObject->computeRepresentation();
	targetColourRepresentationObject->computeRepresentation();

	float distance = referenceColourRepresentationObject->computeDistance(targetColourRepresentationObject);
	cout << representationName << "  distance is: \t" << distance << endl;

	delete referenceColourRepresentationObject;
	delete targetColourRepresentationObject;

	return 1;
}


