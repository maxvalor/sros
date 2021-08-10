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
    node.SetServiceCallback([&](std::string srv_name, std::uint8_t* data, size_t size, std::shared_ptr<pssc::Node::ResponseOperator> op)
    {
        static int i = 0;
        LOG(INFO) << "srv_name:" << srv_name;
        i++;
        op->SendResponse(true, (std::uint8_t*)&i, sizeof(i));
        if (i > 10)
        {
            node.CloseService("a");
        }
    });
    node.Initialize(20001);
    LOG(INFO) << "advertise service: " << (node.AdvertiseService("a") ? "true" : "false");

    getchar();

    return 0;
}
