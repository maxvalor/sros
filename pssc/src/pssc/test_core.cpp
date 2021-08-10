/*
 * test_core.cpp
 *
 *  Created on: Apr 23, 2021
 *      Author: ubuntu
 */


#include "pssc/pssc.h"

#include <glog/logging.h>

//int main()
//{
//    TCPServer server(20001,
//            [](std::shared_ptr<TCPConnection>){
//                LOG(INFO) << "conntecd";
//            },
//            [](std::shared_ptr<TCPConnection>){
//                LOG(INFO) << "disconntecd";
//            });
//    server.Start();
//    return 0;
//}

int main()
{
    pssc::Core core(20001);
    return core.Start();
}

