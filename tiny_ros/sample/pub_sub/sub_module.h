#ifndef SUB_MODULE_H_
#define SUB_MODULE_H_

#include <tiny_ros/tiny_ros.h>
#include <thread>
#include <iostream>
#include "Integer.h"

class SubModule
{
  std::thread t;
  tiny_ros::Subscriber sub;
  tiny_ros::Subscriber sub2;
  tiny_ros::Subscriber sub3;
public:
  SubModule()
  {
    t = std::thread([this]()
    {
      tiny_ros::NodeHandle& handle = tiny_ros::NodeHandle::Instance();

      auto sub_callback = [this](std::shared_ptr<Integer> data1)
      {
        std::cout << "sub_callback: ("
            << data1->data  << ", "
            << std::endl;
        if (data1->data > 100)
        {
            sub.Shutdown();
        }
      };
//      sub = handle.subscribe<Integer>("test", sub_callback);

      auto sub_callback2 = [this](std::shared_ptr<Integer> data1)
      {
        std::cout << "sub_callback2: ("
            << data1->data  << ", "
            << std::endl;
        if (data1->data > 100)
        {
            sub2.Shutdown();
        }
      };
//      sub2 = handle.subscribe<Integer>("test2", sub_callback2);

      auto sub_callback3 = [this](std::shared_ptr<Integer> data1)
      {
        std::cout << "sub_callback3: ("
            << data1->data  << ", "
            << std::endl;
        if (data1->data > 100)
        {
            sub3.Shutdown();
        }
      };
//      sub3 = handle.subscribe<Integer>("test2", sub_callback3);

      /*
      handle.subscribe<int, float, float, int>("test", [](int data1, float data2, float data3, int data4)
      {
        std::cout << "sub_callback: ("
            << data1  << ", "
            << data2  << ", "
            << data3  << ", "
            << data4  << ")"
            << std::endl;

      });

      handle.subscribe<int, float, float, int>("test",
        std::bind(&SubModule::callback, this, std::placeholders::_1
            , std::placeholders::_2
            , std::placeholders::_3
            , std::placeholders::_4)
      );
      */
    });
  }

  void callback(int, float, float, int) {};

  void join()
  {
    t.join();
  }
};


#endif
