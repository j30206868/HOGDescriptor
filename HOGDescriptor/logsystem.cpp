#include "logsystem.h"

CWZImageDebuger CWZImageDebuger::Instance("Configuration");

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
	flag_ = DEBUG_SHOW_AND_PASS;
}
cv::Mat& CWZImageDebugStack::getImage(const std::string &key){
	return imgsmap_[key];
}

int CWZImageDebugStack::size() const{
	return imgsmap_.size();
}

CWZImageDebugStack::FlagState& CWZImageDebugStack::getFlag(){
	return flag_;
}

std::map<std::string, cv::Mat>& CWZImageDebugStack::getImgsmap(){
	return imgsmap_;
}

bool CWZImageDebugStack::isDebugging() const{
	return (flag_ == DEBUG_SHOW_AND_PASS || flag_ == DEBUG_SHOW_AND_WAIT);
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
	if (isDebugging()){
		for (std::map<std::string, cv::Mat>::iterator iter = imgsmap_.begin(); iter != imgsmap_.end(); ++iter)
		{
			if (iter->first == "" || iter->second.empty()) continue;
			cv::namedWindow(iter->first, 0);
			cv::imshow(iter->first, iter->second);
		}
	}
	if (flag_ == DEBUG_SHOW_AND_WAIT){
		pressedkey_ = cv::waitKey(waitms_);
	}
}

/*******************************************
				CWZImageDebuger
********************************************/
CWZImageDebuger::CWZImageDebuger(const std::string name){
	name_ = name;
	cv::namedWindow(name_, 0);
}

static void manageWindows(int value, void *imgdebugstackobj){
	CWZImageDebugStack *obj = (CWZImageDebugStack*)imgdebugstackobj;
	if (!obj->isDebugging()){
		std::map<std::string, cv::Mat> imgsmap = obj->getImgsmap();
		for (std::map<std::string, cv::Mat>::iterator iter = imgsmap.begin(); iter != imgsmap.end(); ++iter)
		{
			cv::destroyWindow(iter->first);
		}
	}
}
void CWZImageDebuger::append(const std::string &key, CWZImageDebugStack &obj){
	obj.setName(key);
	CWZImageDebugStack::FlagState &flag = obj.getFlag();
	flag = CWZImageDebugStack::DEBUG_IDLE;
	cv::createTrackbar(key, 
					   name_, 
					   (int*)&obj.getFlag(), 
					   CWZImageDebugStack::LENGTH-1,
					   manageWindows,
					   &obj);
}
CWZImageDebugStack& CWZImageDebuger::operator[](std::string key) {
	if (imgstackmap_.find(key) == imgstackmap_.end()){//key不存在
		append(key, imgstackmap_[key]);
	}
	return imgstackmap_[key];
}

#endif