#ifndef __READ_LOG_H__
#define __READ_LOG_H__

#include <gtest/gtest.h>
#include <kafka/ckit_kafka.h>
#include <foundation/ckit_regex.h>
#include <foundation/ckit_baseclass.h>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <map>
#include <stdlib.h>

using namespace std;
using namespace ckit;

class readlog : public KafkaConsumerCb
{
public:
	readlog():m_MessageNum(0)
	{
	m_current_time = 0;
	}
	~readlog()
	{
		
	}
	virtual void Process(rd_kafka_message_t* pMessage);
	bool log_regex(const string str);//
	int strtotime(const string strtime);
	bool regex_str(const string str,string& tmp,const string regex);
	void send_curl(queue<string>& queue);
    inline void curl_sprintf(char* c,const char* metric,const char* host,int _time,int value);
public:
	int m_MessageNum;
	int m_current_time;
	queue<string> queue_curl;//发送队列
	std::map<int,int> m_qps_count;
	std::map<int,int> m_rt_count;
	std::map<int,int> m_zero_count;
	std::map<int,int> m_faild_count;
	std::map<int,int> m_discard_count;
};
#endif
