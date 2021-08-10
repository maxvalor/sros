//============================================================================
// Name        : light.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "pssc/protocol/Node.h"
#include <stdio.h>

#define SIZE_A 10

int main(int argc, char*argv[]) {
    pssc::Node node;
    node.Initialize(20001);
    std::uint8_t* data, *resp_data;
    data = new std::uint8_t[SIZE_A];
    size_t resp_size;
    for (int i = 0; i < 10000; ++i)
    {
        LOG(INFO) << "call service start.";
        auto resp = node.RemoteCall("a", data, SIZE_A);
        LOG(INFO) << "call service: " << (resp->success ? "true" : "false");
        if (resp->success)
        {
            int data;
            memcpy(&data, resp->data, resp->sizeOfData);
            LOG(INFO) << "data: " << data;
        }
        else
        {
            break;
        }
    }

    getchar();

    return 0;
}
