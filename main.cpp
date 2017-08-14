#include <kafka/ckit_kafka.h>
#include <iostream>
#include <gtest/gtest.h>
#include "HandleLog.hpp"
using namespace ckit;

int main(void)
{
	//ckit::log::SetPrintLogger();
	/*KafkaConsumer myConsumer;
	HandleLog myhandlelog;
	myConsumer.SetTopicConf("topic.offset.store.method", "file");
	myConsumer.SetTopicConf("topic.offset.store.sync.interval.ms", "1");
	myConsumer.SetTopicConf("auto.commit.interval.ms", "10");
	myConsumer.SetTopicConf("group.id", "g1");
	myConsumer.SetTopic("msearch_cpc");
	myConsumer.SetCb(&myhandlelog);
	myConsumer.Start();
	myConsumer.SetOffset(RD_KAFKA_OFFSET_BEGINNING);
	myConsumer.Join();
	while(1)
	{
		sleep(1);
	}*/
	HandleLog myhandlelog;
	string filepath = "";
	myhandlelog.Test(filepath);
}
