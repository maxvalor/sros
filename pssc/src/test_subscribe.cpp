//============================================================================
// Name        : light.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "pssc/protocol/Node.h"
#include <stdio.h>

int main(int argc, char*argv[]) {
    pssc::Node node;
    int count = 0;
    node.SetTopicCallback([&](std::string topic, std::uint8_t* data, size_t size)
    {
        ++count;
        LOG(INFO) << "topic:" << topic;
        int i;
        memcpy(&i, data, sizeof(int));
        LOG(INFO) << "data:" << i;
        struct timeval tv_start,tv_end;
        gettimeofday(&tv_end, NULL);
        memcpy(&tv_start, data, sizeof(tv_start));
        printf("delay     : %lf ms\n", ((tv_end.tv_sec - tv_start.tv_sec) * 1000 + (tv_end.tv_usec - tv_start.tv_usec) / 1000.0));
        LOG(INFO) << "total size:" << size;
        if (count > 10)
        {
            auto success = node.UnSubscribe("test_topic");
            LOG(INFO) << "unsubscribe rlt value:" << success;
            LOG(INFO) << "unsubscribe: " << (success ? "true" : "false");
        }
    });
    node.Initialize(20001);
    auto success = node.Subscribe("test_topic");
    LOG(INFO) << "subscribe rlt value:" << success;
    LOG(INFO) << "subscribe: " << (success ? "true" : "false");

    getchar();

    return 0;
}
