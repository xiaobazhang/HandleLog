#include "RegexLog.hpp"


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
bool RegexLog::GetLog_Time(int& logtime)
{
	std::string str_log_time;
	if(!m_regex.Compile("(2[0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9])"))
	{
		SET_ERROR_MSG("Compile log time error");
		return false;
	}
	if(!m_regex.Match(m_strlog))
	{
		SET_ERROR_MSG("Match log time error");
		return false;
	}
	if(!m_regex.GetGroupByIdx(0,str_log_time))
	{
		return false;
	}

	logtime = ckit::time::StringTimeToInt(str_log_time);
	return true;
}
bool RegexLog::GetLog_qps()
{
	if(!m_regex.Compile("(query process finish.)"))
	{
		SET_ERROR_MSG("Compile query process finish error");
		return false;
	}
	if(!m_regex.Match(m_strlog))
	{
		return false;
	}
	return true;
}
int RegexLog::GetLog_Cost_Time()
{
	std::string str_cost_time;
	if(!m_regex.Compile("cost_time:([0-9]+)"))
	{
		SET_ERROR_MSG("Compile cost_time error");
		return false;
	}
	if(!m_regex.Match(m_strlog))
	{
		SET_ERROR_MSG("Match cost_time error");
		return -1;
	}
	m_regex.GetGroupByIdx(0,str_cost_time);
	return atoi(str_cost_time.c_str());
}
bool RegexLog::GetLog_Search_Zero()
{
	if(!m_regex.Compile("return adlist size:0"))
	{
		SET_ERROR_MSG("Compile return adlist size:0 error");
		return false;
	}
	if(!m_regex.Match(m_strlog))
	{
		SET_ERROR_MSG("Match return adlist size:0 error");
		return false;
	}
	return true;
}
bool RegexLog::GetLog_Search_Failed()
{
	if(!m_regex.Compile("ret:false"))
	{
		SET_ERROR_MSG("Compile ret:false error");
		return false;
	}
	if(!m_regex.Match(m_strlog))
	{
		SET_ERROR_MSG("Match ret:false error");
		return false;
	}
	return true;
}
bool RegexLog::GetLog_Search_Discard()
{
	if(!m_regex.Compile("discard"))
	{
		SET_ERROR_MSG("Compile discard error");
		return false;
	}
	if(!m_regex.Match(m_strlog))
	{
		SET_ERROR_MSG("Match discard error");
		return false;
	}
	return true;
}
bool RegexLog::GetLog_Ip(std::string& str_ip,const char* ch_kafka_key)
{
	string _str(ch_kafka_key);
	if(!m_regex.Compile("([0-9]{1,3}[.]){3}[0-9]{1,3}"))
	{
		SET_ERROR_MSG("Compile Ip error");
		return false;
	}
	if(!m_regex.Match(_str))
	{
		SET_ERROR_MSG("Match Ip error");
		return false;
	}
	m_regex.GetGroupByIdx(1,str_ip);
	std::cout<<"print kafka ip = "<<str_ip<<std::endl;
	return true;
}
