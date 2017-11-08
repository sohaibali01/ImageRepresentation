#pragma once
#include <iostream>
#include <vector>
#include<fstream>
#include <algorithm> 
#include <cctype>
#include <string>
#include <iterator>  
#include <list>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

class imageRepresentation
{

protected:
	Mat image;
	float* representation ;
	int representationLength;
	float * normL1(const float* rep, int length);

public:
	imageRepresentation(Mat ImagetoBeRepresented);
	imageRepresentation();
	~imageRepresentation();
	const float* getRepresentation(int &outputLength) const;
	virtual void computeRepresentation();
	virtual float computeDistance(imageRepresentation*  targetRepresentation);
};

