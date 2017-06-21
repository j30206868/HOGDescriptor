#ifndef CWZ_LOGSYSTEM_H
#define CWZ_LOGSYSTEM_H

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"

#include <vector>
#include <map>
#include <sstream>

//#define CWZ_DEBUG

#ifdef CWZ_DEBUG
class CWZImageDebugStack{
public:
	CWZImageDebugStack();
	cv::Mat& getImage(const std::string &key);
	int size() const;
	void push(cv::Mat &img);
	void setName(const std::string &str);
	void setWaitKey(int ms);
	void setLastKey(const std::string &key);
	void show(int waitms);
	void show();
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
	CWZImageDebugStack& operator[] (std::string key);
protected:
	std::map<std::string, CWZImageDebugStack> imgstackmap_;
};

inline CWZImageDebugStack& operator<<(CWZImageDebugStack& imggroupsobj, int num){
	imggroupsobj.setWaitKey(num);
	return imggroupsobj;
}
inline CWZImageDebugStack& operator<<(CWZImageDebugStack& imggroupsobj, std::string key){
	imggroupsobj.setLastKey(key);
	return imggroupsobj;
}
inline CWZImageDebugStack& operator<<(CWZImageDebugStack& imggroupsobj, cv::Mat &img){
	imggroupsobj.push(img);
	return imggroupsobj;
}
inline CWZImageDebugStack& operator<<(CWZImageDebuger& logger, std::string key){
	return logger[key];
}

#else
class CWZImageDebugStack{
public:
	CWZImageDebugStack(){}
	inline cv::Mat& getImage(const std::string &key){ return cv::Mat(); }
	inline int size() const{ return 0; }

	inline void push(cv::Mat &img) const{}
	inline void setName(const std::string &str) const{}
	inline void setWaitKey(int ms) const{}
	inline void setLastKey(const std::string &key) const{}
	inline void show(int waitms) const{}
	inline void show() const{}

};

class CWZImageDebuger{
public:
	CWZImageDebugStack& operator[] (std::string key){
		return CWZImageDebugStack();
	}
};

inline CWZImageDebugStack& operator<<(CWZImageDebugStack& imggroupsobj, int num){
	return imggroupsobj;
}
inline CWZImageDebugStack& operator<<(CWZImageDebugStack& imggroupsobj, std::string key){
	return imggroupsobj;
}
inline CWZImageDebugStack& operator<<(CWZImageDebugStack& imggroupsobj, cv::Mat &img){
	return imggroupsobj;
}
inline CWZImageDebugStack& operator<<(CWZImageDebuger& logger, std::string key){
	return CWZImageDebugStack();
}

#endif


#endif //CWZ_LOGSYSTEM_H