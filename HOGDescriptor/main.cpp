#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"

#include <vector>

using namespace std;
using namespace cv;


int main(int argc, char *argv[]){
	Mat img = imread("../bin/images/Koala.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	namedWindow("Image", 0);
	imshow("Image", img);
	waitKey(0);
	
	return 1;
}