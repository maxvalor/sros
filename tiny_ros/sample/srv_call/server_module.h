#ifndef SERVER_MODULE_H_
#define SERVER_MODULE_H_

#include <tiny_ros/tiny_ros.h>
#include <iostream>
#include "TestSrv.h"

class ServerModule
{
public:
  ServerModule()
  {
    tiny_ros::NodeHandle& handle = tiny_ros::NodeHandle::Instance();
    handle.advertiseService<TestSrv>("add_1", [](std::shared_ptr<TestSrv> msg)
    {
      msg->response.data = msg->request.data + 1;
      LOG(INFO) << "value of req:" << msg->request.data;
      return true;
    });
  }

  void join() {}
};

#endif
