#ifndef CWZ_LOGSYSTEM_H
#define CWZ_LOGSYSTEM_H

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"

#include <vector>
#include <map>
#include <sstream>

//#define CWZ_DEBUG

class ImageDebugStack{
public:
	ImageDebugStack(){
		name_ = "";
		lastkey_ = "";
		nonamecount_ = 0;
		waitms_ = 10;
		pressedkey_ = 0;
	}
	cv::Mat& getImage(const std::string &key){
		return imgsmap_[key];
	}
	
	int size() const{
		return imgsmap_.size();
	}

#ifndef CWZ_DEBUG
	void push(cv::Mat &img){
		if (lastkey_ == ""){
			std::stringstream sstm;
			sstm << nonamecount_++;
			imgsmap_[sstm.str()] = img;
		}
		else{
			//不存在的key
			imgsmap_[lastkey_] = img;
		}
	}
	void setName(const std::string &str){
		name_ = str;
	}
	void setWaitKey(int ms){
		waitms_ = ms;
	}
	void setLastKey(const std::string &key){
		lastkey_ = key;
	}
	void show(int waitms){
		waitms_ = waitms;
		this->show();

	}
	void show(){
		for (std::map<std::string, cv::Mat>::iterator iter = imgsmap_.begin(); iter != imgsmap_.end(); ++iter)
		{
			std::string k = iter->first;
			//ignore value
			//Value v = iter->second;
			cv::namedWindow(iter->first, 0);
			cv::imshow(iter->first, iter->second);
		}
		pressedkey_ = cv::waitKey(waitms_);
	}
#else
	void push(cv::Mat &img) const{}
	void setName(const std::string &str) const{}
	void setWaitKey(int ms) const{}
	void setLastKey(const std::string &key) const{}
	void show(int waitms) const{}
	void show() const{}
#endif

protected:
	int nonamecount_;
	int waitms_;
	int pressedkey_;
	std::string name_;
	std::string lastkey_;
	std::map<std::string, cv::Mat> imgsmap_;
};

class CWZImageDebuger{
public:
	ImageDebugStack& operator[] (std::string key) {
		if (imgstackmap_.find(key) == imgstackmap_.end()){//key不存在
			imgstackmap_[key] = ImageDebugStack();
			imgstackmap_[key].setName(key);
		}
		return imgstackmap_[key];
	}
protected:
	std::map<std::string, ImageDebugStack> imgstackmap_;
	
};
#ifndef CWZ_DEBUG
inline ImageDebugStack& operator<<(ImageDebugStack& imggroupsobj, int num){
	imggroupsobj.setWaitKey(num);
	return imggroupsobj;
}
inline ImageDebugStack& operator<<(ImageDebugStack& imggroupsobj, std::string key){
	imggroupsobj.setLastKey(key);
	return imggroupsobj;
}
inline ImageDebugStack& operator<<(ImageDebugStack& imggroupsobj, cv::Mat &img){
	imggroupsobj.push(img);
	return imggroupsobj;
}
inline ImageDebugStack& operator<<(CWZImageDebuger& logger, std::string key){
	return logger[key];
}
#else
inline ImageDebugStack& operator<<(ImageDebugStack& imggroupsobj, int num){
	return imggroupsobj;
}
inline ImageDebugStack& operator<<(ImageDebugStack& imggroupsobj, std::string key){
	return imggroupsobj;
}
inline ImageDebugStack& operator<<(ImageDebugStack& imggroupsobj, cv::Mat &img){
	return imggroupsobj;
}
inline ImageDebugStack& operator<<(CWZImageDebuger& logger, std::string key){
	return ImageDebugStack();
}
#	endif


#endif //CWZ_LOGSYSTEM_H