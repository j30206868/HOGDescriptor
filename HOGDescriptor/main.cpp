#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"

#include <vector>

#include "logsystem.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[]){
	CWZImageDebuger::Instance["Main()"];

	Mat img = imread("../bin/images/Koala.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	namedWindow("Image", 0);
	imshow("Image", img);
	waitKey(0);
	
	cv::Mat blackimg = img.clone();;

	(CWZImageDebuger::Instance["Main()"] << "Koala.jpg" << img << "blackimg" << blackimg).show(0);

	return 1;
}