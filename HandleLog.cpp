#include "HandleLog.hpp"
#include <fstream>

bool SendCurl::Send(std::queue<std::string>& queue)
{
	while(!queue.empty())
	{
		char send_buf[256] ={0};
		sprintf(send_buf,"/usr/local/bin/curl -s -H 'Content-Type: application/json' -m 5 -X POST --data '[%s]' http://127.0.0.1:40001/api/put  -w \"http_code:[%{http_code}]\"",queue.front().c_str());
		//system(send_buf);
		std::cout<<"queue =:"<<queue.front()<<std::endl;
		queue.pop();//出队列
	}
}
void HandleLog::Sprintf_Metric(const string metric,const string host,int _time,int value)
{
	char buf[256] ={0};
	sprintf(buf,"{\"metric\":\"%s\",\"tags\":{\"host\":\"%s\"},\"timestamp\":%d,\"value\":%d}",metric.c_str(),host.c_str(),_time,value);
	string str(buf);
	m_queue.push(str);
}
void HandleLog::Handle(char* logiptime)
{
	string str_kafka_ip;
	if(!m_regexlog.GetLog_Ip(str_kafka_ip,logiptime))
	{
		SET_ERROR_MSG("Get log Ip error");
		return ;
	}
	else
	{
		if(!m_map_date.count(str_kafka_ip))
		{
			map<int,int> _map;
			map<string,map<int,int> > map_tmp;
			map_tmp["qps"] = _map;
			map_tmp["cost_time"] = _map;
			map_tmp["search_zero"] = _map;
			map_tmp["search_failed"] = _map;
			map_tmp["search_discard"] = _map;
			m_map_date[str_kafka_ip] = map_tmp;
		}
		else
		{
			GetData(m_map_date[str_kafka_ip]);
		}
	}
	map<string,map<string,map<int,int> > >::iterator iter;
	for(iter = m_map_date.begin();iter!=m_map_date.end();iter++)
	{
		string str_ip = iter->first;
		if(iter->second["qps"].size()>=20)
		{
			map<int,int>::iterator iter1,iter2,iter3,iter4,iter5;
			iter1 = iter->second["qps"].begin();
			iter2 = iter->second["cost_time"].begin();
			iter3 = iter->second["search_zero"].begin();
			iter4 = iter->second["search_failed"].begin();
			iter5 = iter->second["search_discard"].begin();
			for(int i=0;i<2;i++)//取出map中前两个
			{
				int _avg_costtime = iter2->second/iter1->second;
				if(_avg_costtime > 30)
					_avg_costtime = 7;
				Sprintf_Metric("search_qps_test",str_ip.c_str(),iter1->first,iter1->second);
				Sprintf_Metric("search_rt_test",str_ip.c_str(),iter2->first,_avg_costtime);
				Sprintf_Metric("search_zero_test",str_ip.c_str(),iter3->first,iter3->second);
				Sprintf_Metric("search_fail_test",str_ip.c_str(),iter4->first,iter4->second);
				Sprintf_Metric("search_discard_test",str_ip.c_str(),iter5->first,iter5->second);
				iter->second["qps"].erase(iter1++);
				iter->second["cost_time"].erase(iter2++);
				iter->second["search_zero"].erase(iter3++);
				iter->second["search_failed"].erase(iter4++);
				iter->second["search_discard"].erase(iter5++);
			}
		}
	}	
}
void HandleLog::Test(string filepath)
{
	ifstream inlog(filepath.c_str(),std::ios::in);
	char buf[1024*5] ={0};
	while(inlog.getline(buf,sizeof(buf)))
	{
		string strtmp(buf);
		m_regexlog.setLogStr(strtmp);
		this->Handle("10.17.31.92");
		if(m_queue.size()>20)
		{
			m_sendcurl.Send(m_queue);
		}
	}
}
void HandleLog::Process(rd_kafka_message_t* pMessage)
{
	if(pMessage == NULL)
		return ;
	char* buf = new char[pMessage->len+1];
	memcpy(buf,pMessage->payload,pMessage->len);
	std::string m_kafka_log(buf);
	m_regexlog.setLogStr(m_kafka_log);
	this->Handle((char*)pMessage->key);
	if(m_queue.size()>20)
	{
		m_sendcurl.Send(m_queue);
	}
}
void HandleLog::GetData(std::map<std::string,std::map<int,int> >& mapdate)
{
	int longtime = 0;
	if(!m_regexlog.GetLog_Time(longtime))
		return ;
	else
	{
		if(m_regexlog.GetLog_qps())
		{
			if(mapdate["qps"].count(longtime))
				mapdate["qps"][longtime] = mapdate["qps"][longtime]+1;
			else
				mapdate["qps"][longtime] = 1;
		}
		if(int cost_time = m_regexlog.GetLog_Cost_Time())
		{
			if(-1 != cost_time)
            {
            	if(mapdate["cost_time"].count(longtime))
            		mapdate["cost_time"][longtime] = mapdate["cost_time"][longtime]+cost_time;
            	else
            		mapdate["cost_time"][longtime] = cost_time;
            }
            else
            {
            	SET_ERROR_MSG("cost_time return -1 error");
            }
		}
		if(m_regexlog.GetLog_Search_Zero())
		{
			if(mapdate["search_zero"].count(longtime))
				mapdate["search_zero"][longtime] = mapdate["search_zero"][longtime] + 1;
			else
				mapdate["search_zero"][longtime] = 1;
		}
		else
		{
			mapdate["search_zero"][longtime] = 0;
		}
		if(m_regexlog.GetLog_Search_Failed())
		{
			if(mapdate["search_failed"].count(longtime))
				mapdate["search_failed"][longtime] = mapdate["search_failed"][longtime] + 1;
			else
				mapdate["search_failed"][longtime] = 1;
		}
		else
		{
			mapdate["search_failed"][longtime] = 0;
		}
		if(m_regexlog.GetLog_Search_Discard())
		{
			if(mapdate["search_discard"].count(longtime))
				mapdate["search_discard"][longtime] = mapdate["search_discard"][longtime] + 1;
			else
				mapdate["search_discard"][longtime] = 1;		
		}
		else
		{
			mapdate["search_discard"][longtime] = 0;
		}
	}
}
