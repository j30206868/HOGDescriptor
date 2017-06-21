#include "logsystem.h"

#ifdef CWZ_DEBUG

/*******************************************
				CWZImageDebugStack
********************************************/
CWZImageDebugStack::CWZImageDebugStack(){
	name_ = "";
	lastkey_ = "";
	nonamecount_ = 0;
	waitms_ = 10;
	pressedkey_ = 0;
}
cv::Mat& CWZImageDebugStack::getImage(const std::string &key){
	return imgsmap_[key];
}

int CWZImageDebugStack::size() const{
	return imgsmap_.size();
}

void CWZImageDebugStack::push(cv::Mat &img){
	if (lastkey_ == ""){
		//不存在的key
		std::stringstream sstm;
		sstm << nonamecount_++;
		imgsmap_[sstm.str()] = img;
	}
	else{
		imgsmap_[lastkey_] = img;
		//使用後清除lastkey
		lastkey_ = "";
	}
}
void CWZImageDebugStack::setName(const std::string &str){
	name_ = str;
}
void CWZImageDebugStack::setWaitKey(int ms){
	waitms_ = ms;
}
void CWZImageDebugStack::setLastKey(const std::string &key){
	lastkey_ = key;
}
void CWZImageDebugStack::show(int waitms){
	waitms_ = waitms;
	this->show();

}

void CWZImageDebugStack::show(){
	for (std::map<std::string, cv::Mat>::iterator iter = imgsmap_.begin(); iter != imgsmap_.end(); ++iter)
	{
		if (iter->first=="" || iter->second.empty()) continue;
		cv::namedWindow(iter->first, 0);
		cv::imshow(iter->first, iter->second);
	}
	pressedkey_ = cv::waitKey(waitms_);
}

/*******************************************
				CWZImageDebuger
********************************************/

CWZImageDebugStack& CWZImageDebuger::operator[](std::string key) {
	if (imgstackmap_.find(key) == imgstackmap_.end()){//key不存在
		imgstackmap_[key] = CWZImageDebugStack();
		imgstackmap_[key].setName(key);
	}
	return imgstackmap_[key];
}

#endif