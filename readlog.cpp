#include "readlog.hpp"
//#include <stdlib.h>

void readlog::Process(rd_kafka_message_t* pMessage)
{
	//printf("key = ::%s\n",(char*)pMessage->key);
	if(m_MessageNum == 100000)
	{
		exit(0);
	}
	if(pMessage == NULL)
	{
		printf("pMessage is null\n");
		return ;
	}
	char *buf = new char[pMessage->len*2];
	if(pMessage->payload == NULL)
	{
		printf("message NULL\n");	
	}
	memcpy(buf,(char*)pMessage->payload,pMessage->len);
	string str_log(buf);
	if(queue_curl.size()>=20)
	{
		send_curl(queue_curl);
	}
	log_regex(str_log);
}
int readlog::strtotime(const std::string strtime)
{ 
	struct tm tm_time;  
	int unixtime;  
	strptime(strtime.c_str(),"%Y-%m-%d %H:%M:%S", &tm_time);     
	unixtime = mktime(&tm_time);  
	return unixtime;  
}
bool readlog::regex_str(const string str,string& tmp,const string regex)
{
	ckit::Regex myregex;
	bool result = false;
	result = myregex.Compile(regex);
	result = myregex.Match(str);
	if(result)
	{
		myregex.GetGroupByIdx(0,tmp);
		return true;
	}
	else
	{
		return false;
	}
}
void readlog::curl_sprintf(char* c,const char* metric,const char* host,int _time,int value)
{
	sprintf(c,"{\"metric\":\"%s\",\"tags\":{\"host\":\"%s\"},\"timestamp\":%d,\"value\":%d}",metric,host,_time,value);
}
void readlog::send_curl(queue<string>& queue)
{
	while(!queue.empty())
	{
		char send_buf[256] ={0};
		sprintf(send_buf,"/usr/local/bin/curl -H 'Content-Type: application/json' -m 5 -X POST --data '[%s]' http://127.0.0.1:40001/api/put  -w \"http_code:[%{http_code}]\"",queue.front().c_str());
		system(send_buf);
		//std::cout<<"queue =:"<<queue.front()<<std::endl;
		queue.pop();//出队列
	}
}
bool readlog::log_regex(const string str)
{
	bool result = false;
	int longtime = 0;
	string tmp_time;
	string str_tmp;//用于转换时间
	result = regex_str(str,str_tmp,"(2[0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9])");
	if(result)
	{	
		std::cout<<str_tmp<<std::endl;
		longtime = strtotime(str_tmp);//生成时间
		if(m_qps_count.size()>=20)//如果map中已经有了20秒的值,处理前五秒的数据
		{
				string str_tmp_push;
				char tmp[256] ={0};
				std::map<int,int>::iterator iter1,iter2,iter3,iter4,iter5;
				iter1 = m_qps_count.begin();
				iter2 = m_rt_count.begin();
				iter3 = m_zero_count.begin();
				iter4 = m_faild_count.begin();
				iter5 = m_discard_count.begin();
				for(int i=0;i<5;i++)
				{	
					//std::cout<<iter1->first<<":::"<<iter1->second<<std::endl;
					curl_sprintf(tmp,"search_qps_test","hostname",iter1->first,iter1->second);
					str_tmp_push = tmp;
					queue_curl.push(str_tmp_push);
					memset(tmp,0,sizeof(tmp));
					curl_sprintf(tmp,"search_rt_test","hostname",iter2->first,iter2->second/iter1->second);
					str_tmp_push = tmp;
					queue_curl.push(str_tmp_push);
					memset(tmp,0,sizeof(tmp));
					curl_sprintf(tmp,"search_zero_test","hostname",iter3->first,iter3->second);
					str_tmp_push = tmp;
					queue_curl.push(str_tmp_push);
					memset(tmp,0,sizeof(tmp));
					curl_sprintf(tmp,"search_fail_test","hostname",iter4->first,iter4->second);
					str_tmp_push = tmp;
					queue_curl.push(str_tmp_push);
					memset(tmp,0,sizeof(tmp));
					curl_sprintf(tmp,"search_discard_test","hostname",iter5->first,iter5->second);
					m_qps_count.erase(iter1++);
					m_rt_count.erase(iter2++);
					m_zero_count.erase(iter3++);
					m_faild_count.erase(iter4++);
					m_discard_count.erase(iter5++);
				}	
		}
		str_tmp.clear();
		if(regex_str(str,str_tmp,"(query process finish.)"))
		{
			if(m_qps_count.count(longtime))
			{
				m_qps_count[longtime] = m_qps_count[longtime]+1;
			}
			else
			{
				m_qps_count[longtime] = 1;
			}
		}
		str_tmp.clear();
		tmp_time.clear();
		if(regex_str(str,tmp_time,"cost_time:([0-9]+)"))
		{
			int rt_time = 0;
			rt_time = atoi(tmp_time.c_str());
			if(m_rt_count.count(longtime))
			{
				m_rt_count[longtime] = m_rt_count[longtime] + rt_time;
			}
			else
			{
				m_rt_count[longtime] = rt_time;
			}
		}
		str_tmp.clear();
		if(regex_str(str,str_tmp,"return adlist size:0"))
		{
			if(m_zero_count.count(longtime))
			{
				m_zero_count[longtime] = m_zero_count[longtime] + 1;
			}
			else
			{
				m_zero_count[longtime] = 1;
			}
		}	
		else
		{
			m_zero_count[longtime] = 0;
		}
		str_tmp.clear();
		if(regex_str(str,str_tmp,"ret:false"))
		{
			if(m_faild_count.count(longtime))
			{
				m_faild_count[longtime] = m_faild_count[longtime] +1;
			}
			else
			{
				m_faild_count[longtime] =1;
			}
		}
		else
		{
			m_faild_count[longtime] = 0;
		}
		str_tmp.clear();
		if(regex_str(str,str_tmp,"discard"))
		{
			if(m_discard_count.count(longtime))
			{
				m_discard_count[longtime] = m_discard_count[longtime] + 1;
			}
			else
			{
				m_discard_count[longtime] = 1;
			}
		}
		else
		{
			m_discard_count[longtime] = 0;
		}
	}		
}
