#ifndef CWZ_HOGDESCRIPTOR_H
#define CWZ_HOGDESCRIPTOR_H

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"

#include <vector>
#include <cassert>

/*!
	This class implements Histogram of Oriented Gradient (HOG) feature descriptor in 5 steps as below.
	1. Preprocess
	2. Calculate the Gradient
	3. Calculate Histogram of Oriented Gradients of each cell
	4. Block Normalization
	5. Calculate HOG feature vectors
*/
class HOGDescriptor{
public:
	static const int HOG_BIN_AMOUNT = 9;
public:
	void compute(const cv::Mat &image);
protected:
	/*!
	* @brief how many cells are computed in x and y direction of a target roi image
	*/
	cv::Size cellnum;
	/*!
	* @brief whatever the size of the target roi image is, it will be resized to this fixed imagesize
	*/
	cv::Size imagesize;
	/*!
	* @brief each block contains how many cells in it
	*
	* Limits: blockstride.width
	*/
	cv::Size blockstride;
	/*!
	* @brief the size of each cell in unit of pixel
	*
	* assertion:
	*	imagesize.width % cellnum.width == 0 && imagesize.height % cellnum.height == 0
	* This is going to be recalculated everytime since compute(...) function is called
	*	cellsize = imagesize / cellnum
	*/
	cv::Size cellsize;
	/*!
	* @brief how many blocks are in x and y direction of a target roi image
	*
	* This is going to be recalculated everytime since compute(...) function is called
	*	blocknum = cellnum - blockstride + 1
	*/
	cv::Size blocknum;
	
protected:
	void preprocess(const cv::Mat &inmat, cv::Mat &outmat);
	void gradient(const cv::Mat &inmat, cv::Mat &mag, cv::Mat &angle);
	void hog(const cv::Mat &mag, const cv::Mat &angle, std::vector<std::vector<float>> &cells_features);
	void blocksNormalization(const std::vector<std::vector<float>> &cells_features, cv::Mat &hog_feature_vector);
};

#endif //CWZ_HOGDESCRIPTOR_H