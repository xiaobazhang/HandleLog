#ifndef __REGEX_LOG_H__
#define __REGEX_LOG_H__
#include <foundation/ckit_regex.h>
#include <foundation/ckit_baseclass.h>
#include <foundation/ckit_time.h>
#include <string>
#include <stdlib.h>

using namespace std;
using namespace ckit;


namespace ckit
{
	namespace time
	{
		/* 字符串时间转为时间戳*/
		int StringTimeToInt(std::string str)
		{
			struct tm tm_time;  
			int unixtime;  
			strptime(str.c_str(),"%Y-%m-%d %H:%M:%S", &tm_time);     
			unixtime = mktime(&tm_time);  
			return unixtime; 
		}
	}
}

class RegexLog : public SupportErrorMsg
{
public:
	RegexLog(std::string str_log)
	{
		m_strlog = str_log;
	}
	~RegexLog();
	bool setLogStr(std::string str)
	{
		if(!m_strlog.isEmpty())
			m_strlog.clear();
		m_strlog = str;
		return true;
	}
	bool GetLog_Time(int& logtime);
	virtual bool GetLog_qps();
	virtual int GetLog_Cost_Time();
	virtual bool GetLog_Search_Zero();
	virtual bool GetLog_Search_Failed();
	virtual bool GetLog_Search_Discard();
	virtual bool GetLog_Ip(std::string& str,const char* ch_kafka_key);

private:
	std::string m_strlog;
	ckit::Regex m_regex;
};
