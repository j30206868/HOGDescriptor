#include "hogdescriptor.h"

using namespace std;
using namespace cv;

void HOGDescriptor::compute(const cv::Mat &image){
	assert(!image.empty());
	assert(image.type() == CV_8U && image.channels() == 1);

	assert(imagesize.width > 0 && imagesize.height > 0);
	assert(cellnum.width > 0 && cellnum.height > 0);
	assert(blockstride.width > 0 && blockstride.height > 0);

	assert(blockstride.width <= cellnum.width && blockstride.height <= cellnum.height);

	assert(imagesize.width % cellnum.width == 0);
	assert(imagesize.height % cellnum.height == 0);
	cellsize = Size(imagesize.width / cellnum.width, imagesize.height / cellnum.height);
	blocknum = Size(cellnum.width - blockstride.width + 1, cellnum.height - blockstride.height + 1);

	Mat hog_feature_vector(1, blocknum.area() * blockstride.area() * HOG_BIN_AMOUNT, CV_32FC1);

	Mat img = image.clone();
	Mat mag, angle;
	std::vector<std::vector<float>> cells_features;
	preprocess(img, img);
	gradient(img, mag, angle);
	hog(mag, angle, cells_features);
	blocksNormalization(cells_features, hog_feature_vector);
}

void HOGDescriptor::preprocess(const cv::Mat &inmat, cv::Mat &outmat){
	resize(inmat, outmat, imagesize);
	outmat.convertTo(outmat, CV_32F, 1 / 255.0);
}

void HOGDescriptor::gradient(const cv::Mat &inmat, cv::Mat &mag, cv::Mat &angle){
	Mat gx, gy;
	Sobel(inmat, gx, CV_32F, 1, 0, 1);
	Sobel(inmat, gy, CV_32F, 0, 1, 1);
	cartToPolar(gx, gy, mag, angle, true);
}

/*!
	This is a local subfunction of HOGDescriptor::hog(...) which apply HOG on a specified cell
*/
static void hogOnACell(const int x1, const int y1, const Size &cellsize, 
					const cv::Mat &mag, const cv::Mat &angle, std::vector<std::vector<float>> &cells_features)
{
	const static float max_angle = 180;
	const static int total_bin_amount = HOGDescriptor::HOG_BIN_AMOUNT;
	assert((int)max_angle % total_bin_amount == 0);
	const static int index_divider = cvRound( max_angle / total_bin_amount );
	//lower_index_of_bin = floor(angle / index_divider)
	//up_index_of_bin = lower_index_of_bin + 1
	//In case of the angles >= 170, the up index of bin will over the max index value.
	//For convenience, we increase the size of buffer by one and keeps mag value for bin 1 temporarily (ghist[0])
	//To avoid over the index of the limit, we expand buffer size by one to temporarily keep mag values for bin 1 (ghist[0]) 
	const static int ghist_len = total_bin_amount + 1;
	static float ghist[ghist_len];
	//reinit
	memset(ghist, 0, sizeof(float) * ghist_len);
	for (int y = y1; y < y1 + cellsize.height; y++){
		float *magrow = (float*)mag.ptr<float>(y);
		float *angrow = (float*)angle.ptr<float>(y);
		for (int x = x1; x < x1 + cellsize.width; x++){
			float angle_in_unit_of_bin = angrow[x] / index_divider;
			int lower_bin_index = floor(angle_in_unit_of_bin);
			int up_bin_index = lower_bin_index + 1;
			float lower_weight = (up_bin_index - angle_in_unit_of_bin);

			ghist[lower_bin_index] += magrow[x] * lower_weight;
			ghist[up_bin_index]    += magrow[x] * (1 - lower_weight);
		}
	}
	ghist[0] += ghist[ghist_len-1];

	std::vector<float> feature;
	for (int i = 0; i < total_bin_amount; i++){
		feature.push_back(ghist[i]);
	}
	cells_features.push_back(feature);
}
/*!
	hog(...) will compute the raw Histogram of Oriented Gradient(HOG) features for each cell of target roi image.

	mag, angle:
		outcomes of previous step gradient(...)
	cells_features:
		it is used to store features of each cell for the result of hog on target roi image
		- cells_features.size() == cellnum.area()
		- cells_features[n].size() == 9 (9 bins of Histogram of Oriented Gradient)
		- features are calculated and push in the order of the following:
			(x axis first and then y axis)
			1	2	3	4	...	  cellnum.width
			.							.
			.							.
			.							.
			cellnum.height	...	 cellnum.width * cellnum.height
*/
void HOGDescriptor::hog(const cv::Mat &mag, const cv::Mat &angle, std::vector<std::vector<float>> &cells_features){
	//calculate features for every cell
	for (int y = 0; y < cellnum.height; y++){
		for (int x = 0; x < cellnum.width; x++){
			int x1 = x * cellsize.width;
			int y1 = y * cellsize.height;
			hogOnACell(x1, y1, cellsize, mag, angle, cells_features);
		}
	}
}

///block normalization
inline float blockFeaturesSum(int x1, int y1, const cv::Size &blockstride, const cv::Size &cellnum, const std::vector<float> &featuressum){
	float sum = 0;
	for (int y = y1; y < y1 + blockstride.height; y++){
		int baseidx = y * cellnum.width;
		for (int x = x1; x < x1 + blockstride.width; x++){
			sum += featuressum[baseidx + x];
		}
	}
	return sum;
}

inline void produceHOGFeatureVector(int x1, int y1, float denominator, const cv::Size &blocknum, const cv::Size &blockstride, 
									const cv::Size &cellnum, const std::vector<float> &featuressum, cv::Mat &feature_vector)
{
	int block_index = y1 * blocknum.width + x1;
	int final_feature_index = block_index * blockstride.area();
	for (int y = y1; y < y1 + blockstride.height; y++){
		int baseidx = y * cellnum.width;
		for (int x = x1; x < x1 + blockstride.width; x++){
			float norm_feature = featuressum[baseidx + x] / denominator;
			feature_vector.at<float>(final_feature_index) = norm_feature;
		}
	}
}

void HOGDescriptor::blocksNormalization(const std::vector<std::vector<float>> &cells_features, cv::Mat &hog_feature_vector){
	std::vector<float> featuressum;
	for (int i = 0; i < cells_features.size(); i++){
		float sum = 0;
		for (int j = 0; j < cells_features[i].size(); j++){
			sum += cells_features[i][j];
		}
		featuressum.push_back(sum);
	}
	for (int y = 0; y < blocknum.height; y++){
		for (int x = 0; x < blocknum.width; x++){
			float sum = blockFeaturesSum(x, y, blockstride, cellnum, featuressum);
			produceHOGFeatureVector(x, y, sum, blocknum, blockstride, cellnum, featuressum, hog_feature_vector);
		}
	}
}