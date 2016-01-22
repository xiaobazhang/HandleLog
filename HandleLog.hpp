#ifndef __HANDLE_LOG_H__
#define __HANDLE_LOG_H__
#include <foundation/ckit_baseclass.h>
#include <foundation/ckit_basedef.h>
#include <kafka/ckit_kafka.h>
#include "RegexLog.hpp"
#include <queue>
#include <iostream>
#include <map>

using namespace ckit;
using namespace std;
class SendCurl: public SupportErrorMsg
{
public:
	SendCurl()
	{
	}
	~SendCurl()
	{
	}
	bool Send(std::queue<std::string>& queue);
};

 class HandleLog: public KafkaConsumerCb,public SupportErrorMsg 
 {
 public:
 	HandleLog()
	{
	}
 	~HandleLog()
	{
	}
	void Test();
 	void Handle(char* logiptime);
 	virtual void GetData(std::map<std::string,std::map<int,int> >& mapdate);
 	 inline void  Sprintf_Metric(const string metric,const string host,int _time,int value);
 	virtual void Process(rd_kafka_message_t* pMessage);
 private:
 	SendCurl m_sendcurl;
 	std::queue<std::string> m_queue;
 	RegexLog m_regexlog;
 	map<string,map<string,map<int,int> > > m_map_date;
 	//std::string m_str_log;
 };
#endif
