#ifndef PUB_MODULE_H_
#define PUB_MODULE_H_


#include <thread>
#include <iostream>
#include <sys/time.h>
#include <tiny_ros/tiny_ros.h>
#include "Integer.h"

class PubModule
{
  std::thread t;

public:
  PubModule()
  {
    t = std::thread([]()
    {
      tiny_ros::NodeHandle& handle = tiny_ros::NodeHandle::Instance();
      auto pub = handle.advertise<Integer>("test");
      auto pub2 = handle.advertise<Integer>("test2");

      int i = 0;
      while (++i < 10000)
      {
        auto msg = std::make_shared<Integer>();
        msg->data = i;
        pub->Publish(msg);
        pub2->Publish(msg);
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
