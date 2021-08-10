#ifndef CLIENT_MODULE_H_
#define CLIENT_MODULE_H_

#include <tiny_ros/tiny_ros.h>
#include <thread>
#include <iostream>
#include "TestSrv.h"

class ClientModule
{
  std::thread t;

public:
  ClientModule()
  {
    t = std::thread([]()
    {
      int i = 0;

      while (++i)
      {
        tiny_ros::NodeHandle& handle = tiny_ros::NodeHandle::Instance();
        auto client = handle.serviceClient<TestSrv>("add_1");
        if (client.isValid())
        {
          auto srv = std::make_shared<TestSrv>();
          srv->request.data = i;
          if (client.Call(srv))
          {
              std::cout << "value of req:" << i << std::endl;
            std::cout << "value of resp:" << srv->response.data << std::endl;
          }
          else
          {
            std::cout << "return false." << std::endl;
          }
        }
        else
        {
          std::cout << "error of srv." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });
  }

  void join()
  {
    t.join();
  }
};


#endif
